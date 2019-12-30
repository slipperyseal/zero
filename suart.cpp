//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute		Dirk Mahoney			dirk@tcri.com.au
// Catchpole Robotics					Christian Catchpole		christian@catchpole.net
//


#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "thread.h"
#include "memory.h"
#include "doublebuffer.h"
#include "suart.h"


using namespace zero;


namespace {
    SuartTx* _suartTx = 0UL;
}


SuartTx::SuartTx()
{
    _suartTx = this;
}


SuartTx::~SuartTx()
{
    disable();
    _suartTx = 0UL;
}


uint16_t SuartTx::formatForSerial(const uint8_t d)
{
    uint16_t rc = 0UL;

    rc = d << 1;
    rc &= ~(1L << 0);             // force start bit low
    rc |= (1L << 9);              // stop bit high (so it ends high)

    return rc;
}

void SuartTx::startTxTimer()
{
    #define SCALE(x) ((F_CPU * (x)) / 16000000ULL)      // to scale the Timer for MCU clock speed

    TCCR2B = 0;                                         // make sure timer is stopped
    TCNT2 = 0;                                          // reset the counter
    TCCR2A = (1 << WGM21);                              // CTC
    OCR2A = SCALE(500000ULL / _baud) - 1;               // scaled for F_CPU
    TIMSK2 |= (1 << OCIE2A);                            // enable Timer2 ISR
    TCCR2B = ((1 << CS21) | (1 << CS20));               // start Timer2 with pre-scaler 32
}


void SuartTx::stopTxTimer()
{
    TIMSK2 &= ~(1 << OCIE2A);       // disable Timer ISR
    TCCR2B = 0;                     // make sure timer is stopped
    TCNT2 = 0;                      // reset the counter
}


bool SuartTx::enable(const Synapse txCompleteSyn)
{
    _txCompleteSyn = txCompleteSyn;

    *_ddr |= _pinMask;          // output
    *_port |= _pinMask;         // idle-high

    return true;
}


void SuartTx::disable()
{
    stopTxTimer();

    if (_ddr && _port && _pinMask) {
        *_ddr &= ~_pinMask;
        *_port &= ~_pinMask;
    }
}


void SuartTx::setCommsParams(
    const uint32_t baud,
    volatile uint8_t* ddr,
    volatile uint8_t* port,
    const uint8_t pin)
{
    disable();

    _baud = baud;
    _ddr = ddr;
    _port = port;
    _pinMask = (1 << pin);
}


bool SuartTx::transmit(const void* buffer, const uint16_t sz)
{
    if (_txBuffer) return false;
    if (!buffer) return false;
    if (!sz) return false;

    // prime the buffer data
    _txBuffer = (uint8_t*) buffer;
    _txSize = sz;

    // remember the buffer data
    _txBuffer = (uint8_t*) buffer;
    _txSize = sz;

    // enable the ISR that starts the transmission
    startTxTimer();

    return true;
}


bool SuartTx::getNextTxByte(uint8_t& data)
{
    bool rc = false;

    data = 0;

    if (_txSize) {
        data = *_txBuffer++;
        _txSize--;

        rc = true;
    }

    return rc;
}


ISR(TIMER2_COMPA_vect)
{
    // just in time fetch of data to send
    if (!_suartTx->_txReg) {
        uint8_t nextByte;

        if (!_suartTx->getNextTxByte(nextByte)) {
            // switch off transmission
            _suartTx->stopTxTimer();

            // signal and tidy up
            _suartTx->_txCompleteSyn.signal();
            _suartTx->_txBuffer = 0UL;

        } else {
            // load next byte
            _suartTx->_txReg = _suartTx->formatForSerial(nextByte);
        }
    }

    if (_suartTx->_txReg) {
        // we're mid-byte, keep pumping out the bits

        if (_suartTx->_txReg & 1) {
            *_suartTx->_port |= _suartTx->_pinMask;

        } else {
            *_suartTx->_port &= ~_suartTx->_pinMask;
        }

        _suartTx->_txReg >>= 1;
    }
}