//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


#include "thread.h"


using namespace zero;


// startup_sequence() is your new main().
// Initialize any GPIO or other things that make sense to do so here.
// NOTE: You are not inside a Thread here. This functions is called
// before zero has initialized itself fully. Consequently, you cannot
// call many zero functions from here. If you have enabled the DEBUG_
// options in the makefile, you can call debug::print() here.
void startup_sequence()
{
    ;
}


// This is the Thread that will run when no other Thread wants to. For
// this reason, you cannot block in the idle thread. You must be busy
// the whole time. This function has been exposed to the developer so
// that you can change the idle thread to perhaps flash an LED, or
// send some debugging text - something to let you know when the idle
// thread is running. Most of the time, you can use the supplied idle
// thread code and leave it that.
int idleThreadEntry()
{
    while (true);
}
