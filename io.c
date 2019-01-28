#include <stdint.h>
#include <stdbool.h>

#include "io.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define BTN_INT(idx, port, pin) {\
	btn_hists[(idx)] = (btn_hists[(idx)] << 1) | (((port) & (1 << (pin))) >> (pin));\
	\
	if (btn_hists[(idx)] == 0xff) {\
		btn_states[(idx)] = true;\
	} else if (btn_hists[(idx)] == 0x00) {\
		btn_states[(idx)] = false;\
	}\
}

volatile uint64_t overflow_count;
volatile uint8_t btn_hists[7];
volatile bool btn_states[7];

ISR(TIMER0_OVF_vect) {
	BTN_INT(BTN_LEFT, PINF, PF6);
	BTN_INT(BTN_RIGHT, PINF, PF5);

	BTN_INT(BTN_DPAD_LEFT, PINB, PB1);
	BTN_INT(BTN_DPAD_RIGHT, PIND, PD0);
	BTN_INT(BTN_DPAD_UP, PIND, PD1);
	BTN_INT(BTN_DPAD_DOWN, PINB, PB7);
	BTN_INT(BTN_DPAD_CENTER, PINB, PB0);
}

ISR(TIMER1_OVF_vect) {
	overflow_count++;
}

void io_set_led(uint8_t led, bool on) {
	switch (led) {
	case LED_LEFT:
		if (on) {
			PORTB |= (1 << PB2);
		} else {
			PORTB &= ~(1 << PB2);
		}

		break;
	case LED_RIGHT:
		if (on) {
			PORTB |= (1 << PB3);
		} else {
			PORTB &= ~(1 << PB3);
		}

		break;
	case LED_TEENSY:
		if (on) {
			PORTD |= 1 << PD6;
		} else {
			PORTD &= ~(1 << PD6);
		}

		break;
	case LED_LCD:
		if (on) {
			PORTC &= ~(1 << PC7);
		} else {
			PORTC |= 1 << PC7;
		}

		break;
	default:
		return;
	}
}

bool io_get_btn(uint8_t btn) {
	return btn_states[btn];
}

bool io_get_btn_block(uint8_t btn) {
	if (btn_states[btn]) {
		while (btn_states[btn]);

		return true;
	}

	return false;
}

uint8_t io_get_joystick() {
	if (io_get_btn(BTN_DPAD_CENTER)) {
		return BTN_DPAD_CENTER;
	}

	if (io_get_btn(BTN_DPAD_UP)) {
		return BTN_DPAD_UP;
	}

	if (io_get_btn(BTN_DPAD_RIGHT)) {
		return BTN_DPAD_RIGHT;
	}

	if (io_get_btn(BTN_DPAD_DOWN)) {
		return BTN_DPAD_DOWN;
	}

	if (io_get_btn(BTN_DPAD_LEFT)) {
		return BTN_DPAD_LEFT;
	}

	return BTN_NONE;
}

uint8_t io_get_joystick_block() {
	if (io_get_btn_block(BTN_DPAD_CENTER)) {
		return BTN_DPAD_CENTER;
	}

	if (io_get_btn_block(BTN_DPAD_UP)) {
		return BTN_DPAD_UP;
	}

	if (io_get_btn_block(BTN_DPAD_RIGHT)) {
		return BTN_DPAD_RIGHT;
	}

	if (io_get_btn_block(BTN_DPAD_DOWN)) {
		return BTN_DPAD_DOWN;
	}

	if (io_get_btn_block(BTN_DPAD_LEFT)) {
		return BTN_DPAD_LEFT;
	}

	return BTN_NONE;
}

uint16_t io_get_wheel(uint8_t wheel) {
	if (wheel != WHEEL_LEFT && wheel != WHEEL_RIGHT) {
		return 0;
	}

	ADMUX &= ~7;
	ADMUX |= wheel & 7;

	ADCSRA |= (1 << ADSC);

	while (ADCSRA & (1 << ADSC));

	return ADC;
}

void io_init() {
	if (F_CPU == 8000000UL) {
		// Set clock speed to 8 MHz
		CLKPR = 0x80;
		CLKPR = 0x01;
	} else {
		// Set clock speed to 16 MHz
		CLKPR = 0x80;
		CLKPR = 0x00;
	}

	DDRB = (1 << PB2) | (1 << PB3);
	DDRC = (1 << PC7);
	DDRD = (1 << PD6);
	DDRF = 0x00;

	PORTB = 0x00;

	// Timer0 - Debouncing
	// Prescale /256 ~244 Hz
	TCCR0B |= (4 << CS00);

	// Interrupt on overflow
	TIMSK0 |= (1 << TOIE0);

	// Timer1 - System time
	// Prescale /1024
	TCCR1B |= (5 << CS10);

	// Interrupt on overflow
	TIMSK1 |= (1 << TOIE1);

	// Enable global interrupts
	sei();

	// ADC
	// Prescale /64
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

double systime() {
	uint64_t oval;
	uint16_t cval;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		oval = overflow_count;
		cval = TCNT1;
	}
	
	//TODO
	return (double)(oval * 0x10000 + cval) * 1024.0f / (double)F_CPU;
}
