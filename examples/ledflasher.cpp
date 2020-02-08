//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


#include "ledflasher.h"
#include "thread.h"
#include "gpio.h"


using namespace zero;


LedFlasher::LedFlasher(
    const PinField ledPins,
    const uint32_t delayMs,
    const int count)
:
    // call parent ctor, with entryPoint as a lambda.
    // This is a stub that just calls ::main()
    Thread(0, []()
    {
        return ((LedFlasher&) me).main();
    }),

    // other params
    _ledPins{ledPins},
    _delayMs{delayMs},
    _flashesRemaining{count}
{
    // ctor body
}


// the main body of the Thread
int LedFlasher::main()
{
    Gpio led(_ledPins);

    if (!led) {
        return 20;
    }

    led.setAsOutput();
    
    while (true) {
        led.switchOn();
        me.wait(0UL, _delayMs);

        led.switchOff();
        me.wait(0UL, _delayMs);

        if (_flashesRemaining > 0) {
            _flashesRemaining--;

            if (!_flashesRemaining) {
                break;
            }
        }
    }

    return 0;
}