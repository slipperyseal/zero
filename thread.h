/*
 * zero - pre-emptive multitasking kernel for AVR
 *
 *  Techno Cosmic Research Institute	Dirk Mahoney			dirk@tcri.com.au
 *  Catchpole Robotics					Christian Catchpole		christian@catchpole.net
 * 
 */

#ifndef TCRI_ZERO_THREAD_H
#define TCRI_ZERO_THREAD_H


#include "zero_config.h"

#include <stdint.h>
#include <avr/pgmspace.h>
#include "namedobject.h"


#define ZERO_BUILD_VERSION 0
#define ZERO_BUILD_REVISION 5

static const uint8_t THREAD_MIN_STACK_BYTES = 48;

namespace zero {

	typedef int (*ThreadEntryPoint)();

	enum ThreadState {
		TS_RUNNING,
		TS_READY,
		TS_PAUSED,
		TS_WAITING,
		TS_TERMINATED,
		TS_WAIT_TERM,
		TS_WAIT_ATOMIC_WRITE,
		TS_WAIT_READ,
		TS_WAIT_WRITE,
	};

	// Thread Launch Flags - these control various aspects
	// of how a Thread starts up and shuts down

	const uint16_t TLF_READY = 1;				// Thread is ready to run as soon as the scheduler allows
	const uint16_t TLF_AUTO_CLEANUP = 2;		// Thread will clean up after itself upon termination
	const uint16_t TLF_QUICK = 4;				// Quick launch - bypass niceties like clearing stack memory
	const uint16_t TLF_POOL = 8;				// Thread is part of a Thread Pool. Tells the cleanup to recycle.

	// Thread class
	class Thread {
	public:
		// kickstarts the scheduler
		static void init();

		// Thread creation
		Thread(const uint16_t stackSizeBytes, const ThreadEntryPoint entry, const int flags = TLF_READY | TLF_AUTO_CLEANUP);
		Thread(const char* name, const uint16_t stackSizeBytes, const uint8_t quantumOverride, const ThreadEntryPoint entryPoint, const int flags = TLF_READY | TLF_AUTO_CLEANUP);

		// general blocking and unblocking
		static void block(const ThreadState newState, const uint32_t blockInfo);
		static void unblock(const ThreadState state, const uint32_t blockInfo);

		// for applications to find the current Thread
		static Thread* me();

		// Thread management
		static void forbid();
		static void permit();
		static bool isSwitchingEnabled();

		bool setParameter(const uint8_t parameterNumber, const uint16_t value);
		bool run();
		bool pause();
		int join();
		bool remove();
		bool cleanup();
		void waitUntil(const uint32_t untilMs);

		// Properties (thread_info.cpp)
		uint16_t getThreadId();
		uint16_t getStackBottom();
		uint16_t getStackTop();
		uint16_t getStackSizeBytes();
		uint16_t calcCurrentStackBytesUsed();

#ifdef INSTRUMENTATION
		uint16_t calcPeakStackBytesUsed();
#endif
		// end (thread_info.cpp)

		// miscellaneous, but sort of related!
		static uint32_t now();

		// ********* DATA *********

		NamedObject _systemData;							// NamedObject must be first
		Thread* _prev;										// Thread lists previous
		Thread* _next;										// Thread lists next

		uint8_t _sreg;										// status register
		uint16_t _sp;										// stack pointer

	#ifdef RAMPZ
		uint8_t _rampz;										// RAMPZ
	#endif

		// main control block
		uint16_t _tid;										// Thread ID
		ThreadState _state;									// current state
		uint32_t _blockInfo;								// info related to current state
		int (*_entryPoint)();								// main entry for the Thread
		uint8_t _quantumTicks;								// the size of this Thread's quantum
		uint8_t _remainingTicks;							// time left in the current quantum
		uint16_t _launchFlags;								// special flags
		int _exitCode;										// return code upon termination

	#ifdef INSTRUMENTATION
		uint32_t _ticks;									// total ticks received
		uint16_t _lowestSp;									// lowest stack pointer position seen
	#endif

	private:
		void configureThread(const char* name, uint8_t* stack, const uint16_t stackSize, const uint8_t quantumOverride, const ThreadEntryPoint entryPoint, const uint16_t flags);
		void prepareStack(uint8_t* stack, const uint16_t stackSize, const bool quick);
		static Thread* createIdleThread();

		uint8_t* _stackBottom;								// the address of the lowest end of the stack
		uint16_t _stackSizeBytes;							// the size of the stack, in bytes
	};

}


#define delay(ms) { Thread::me()->waitUntil(Thread::now() + (ms)); }


#endif
