/*
 * zero - pre-emptive multitasking kernel for AVR
 *
 *  Techno Cosmic Research Institute	Dirk Mahoney			dirk@tcri.com.au
 *  Catchpole Robotics					Christian Catchpole		christian@catchpole.net
 *
 * 	BLINK - Flashes two LEDs, connected to PB4/5
 * 
 */

#include <avr/io.h>
#include <util/delay.h>
#include "zero_config.h"
#include "thread.h"
#include "cli.h"

using namespace zero;

#define LED_PORT PORTB
#define LED_DDR DDRB
#define LED_PIN_1 PINB4
#define LED_PIN_2 PINB5

int first() {
	while (true) {
		LED_PORT ^= (1 << LED_PIN_1);
		_delay_ms(250);
	}
}

int second() {
	while (true) {
		LED_PORT ^= (1 << LED_PIN_2);
		_delay_ms(330);
	}
}

void startup_sequence() {
	// setting up GPIO
	LED_DDR = (1 << LED_PIN_1) | (1 << LED_PIN_2);

	// the main threads

#ifdef CLI_ENABLED
	new Thread(PSTR("cli"), CLI_STACK_BYTES, 50, cliMain, TLF_READY | TLF_AUTO_CLEANUP);
#endif

	new Thread(PSTR("first"), 0, 0, first, TLF_READY | TLF_AUTO_CLEANUP);
	new Thread(PSTR("second"), 0, 0, second, TLF_READY | TLF_AUTO_CLEANUP);
}
