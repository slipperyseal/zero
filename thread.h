//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute		Dirk Mahoney			dirk@tcri.com.au
// Catchpole Robotics					Christian Catchpole		christian@catchpole.net
//


#ifndef TCRI_ZERO_THREAD_H
#define TCRI_ZERO_THREAD_H


#include <stdint.h>


namespace zero {

    typedef int (*ThreadEntry)();

    typedef uint16_t SignalField;
    typedef uint16_t ThreadFlags;

    const ThreadFlags TF_NONE = 0;
    const ThreadFlags TF_READY = (1L << 0);
    const ThreadFlags TF_SELF_DESTRUCT = (1L << 1);
    const ThreadFlags TF_FIRE_AND_FORGET = TF_READY | TF_SELF_DESTRUCT;
    
    class Thread {
    public:
        // Life cycle
        static Thread& getCurrentThread();
        static uint64_t now();

        static void forbid();
        static void permit();
        static bool isSwitchingEnabled();

        Thread(const uint16_t stackSize, const ThreadEntry entry, const ThreadFlags flags = TF_READY, const SignalField termSigs = 0UL, uint16_t* exitCode = 0UL);
        
        // Signals
        SignalField allocateSignal(const uint16_t reqdSignalNumber = -1);
        void freeSignals(const SignalField signals);

        SignalField getCurrentSignals();
        SignalField clearSignals(const SignalField sigs);
        
        SignalField wait(const SignalField sigs);
        void signal(const SignalField sigs);

        // Don't touch! That means you! :)
        #include "thread_private.h"
    };

    // the target of a signal. Is a Thread/SignalField pair.
    struct Synapse {
        Thread* thread;
        SignalField signals;

        Synapse() {}

        Synapse(const SignalField sigs) {
            thread = &Thread::getCurrentThread();
            signals = sigs;
        }

        void clear() {
            thread = 0UL;
            signals = 0UL;
        }

        bool isValid() {
            return (thread != 0UL && signals != 0UL);
        }

        void signal() {
            if (isValid()) {
                thread->signal(signals);
            }
        }

        void clearSignals() {
            if (isValid()) {
                thread->clearSignals(signals);
            }
        }

        SignalField wait() {
            if (!isValid() || &Thread::getCurrentThread() != thread) return 0;
            return Thread::getCurrentThread().wait(signals);
        }

    }; 
}


#define me Thread::getCurrentThread()


#pragma region ********* ATOMIC BLOCK


// Funky little ATOMIC_BLOCK macro clones for context switching
static __inline__ uint8_t __iForbidRetVal() {
    zero::Thread::forbid();
    return 1;
}


static __inline__ void __iZeroRestore(const uint8_t* __tmr_save) {
    if (*__tmr_save) {
        zero::Thread::permit();
    }
}


#define ZERO_ATOMIC_BLOCK(t)        for ( t, __ToDo = __iForbidRetVal(); __ToDo ; __ToDo = 0 )
#define ZERO_ATOMIC_RESTORESTATE    uint8_t tmr_save __attribute__((__cleanup__(__iZeroRestore))) = (uint8_t)(zero::Thread::isSwitchingEnabled())
#define ZERO_ATOMIC_FORCEON         uint8_t tmr_save __attribute__((__cleanup__(__iZeroRestore))) = (uint8_t) 1)


#pragma endregion


#pragma region ********* SIGNALS BLOCK


// Funky little ATOMIC_BLOCK macro clones for allocating and freeing Signals
static __inline__ void __iSignalsRestore(const zero::SignalField* __signalsToFree) {
    if (*__signalsToFree) {
        zero::Thread::getCurrentThread().freeSignals(*__signalsToFree);
    }
}


#define ZERO_SIGNAL(n)      for ( SignalField n __attribute__((__cleanup__(__iSignalsRestore))) = (SignalField)(zero::Thread::getCurrentThread().allocateSignal()), __ToDo = 1; __ToDo ; __ToDo = 0 )


#pragma endregion


#endif
