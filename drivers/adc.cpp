//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


#ifdef ZERO_DRIVERS_ADC


#include <avr/interrupt.h>
#include <util/atomic.h>

#include "adc.h"
#include "resource.h"


using namespace zero;


namespace {

    Adc* _currentAdc{ nullptr };

}    // namespace


/// @brief Creates a new Adc object
/// @param syn A Synapse to signal when each conversion is complete.
Adc::Adc( const Synapse& syn )
:
    _readySyn{ syn },
    _lastConversion{ 0 }
{
    ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ) {
        if ( !_currentAdc ) {
            if ( resource::obtain( resource::ResourceId::Adc ) ) {
                _currentAdc = this;
            }
        }
    }
}


// dtor
Adc::~Adc()
{
    ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ) {
        if ( _currentAdc == this ) {
            disable();
            _readySyn.clearSignals();
            resource::release( resource::ResourceId::Adc );
            _currentAdc = nullptr;
        }
    }
}


/// @brief Determines if the Adc initialized correctly
/// @returns `true` if the Adc resource was successfully acquired, `false` otherwise.
Adc::operator bool() const
{
    return _currentAdc == this;
}


/// @brief Enables the ADC sampling peripheral
void Adc::enable()
{
    ADMUX = ( 1 << REFS0 );                             // AVcc reference
    ADCSRA |= ( 1 << ADIE );                            // enable ADC ISR
    ADCSRA |= ( 1 << ADEN ) | ( 7 << ADPS0 );           // switch on ADC and ADC clock
}


/// @brief Disables the ADC sampling peripheral
void Adc::disable()
{
    ADCSRA &= ~( 1 << ADIE );                           // disable ADC ISR
    ADCSRA &= ~( 1 << ADEN );                           // switch off the ADC circuitry
}


/// @brief Begins an ADC conversion on a given channel
/// @param channel The channel number to sample.
void Adc::beginConversion( const uint8_t channel )
{
    ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ) {
        // wait for previous conversion to  finish
        while ( ADCSRA & ( 1 << ADSC ) )
            ;

        // we're no longer ready
        _readySyn.clearSignals();

        // select the correct pin
        ADMUX = ( ADMUX & 0b11111000 ) | ( channel & 0b111 );

        // start the conversion
        ADCSRA |= ( 1 << ADSC );
    }
}


/// @brief Sets the value of the last conversion
/// @param v The value to store for the last conversion.
void Adc::setLastConversion( const uint16_t v )
{
    _lastConversion = v;
    _readySyn.signal();
}


/// @brief Gets the value of the last conversion
/// @returns A `uint16_t` that holds the 10-bit value of the last conversion.
uint16_t Adc::getLastConversion() const
{
    ATOMIC_BLOCK ( ATOMIC_RESTORESTATE ) {
        return _lastConversion;
    }
}


ISR( ADC_vect )
{
    if ( _currentAdc ) {
        // set the read value into the current ADC object
        const volatile uint16_t reading{ ADC };
        _currentAdc->setLastConversion( reading );
    }
}


#endif    // ZERO_DRIVERS_ADC
