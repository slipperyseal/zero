//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


public:
    bool getNextTxByte(uint8_t& data);
    void startTxTimer();
    void stopTxTimer();
    uint16_t formatForSerial(uint8_t data);

    // buffer-level stuff
    uint8_t* _txBuffer = 0UL;
    uint16_t _txSize = 0UL;
    Synapse _txReadySyn;

    // sub-byte management
    uint16_t _txReg = 0UL;

    // GPIO and comms
    uint32_t _baud = 0ULL;
    volatile uint8_t* _ddr = 0UL;
    volatile uint8_t* _port = 0UL;
    uint8_t _pinMask = 0;