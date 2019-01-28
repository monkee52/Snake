// Code based off cab202_teensy library

#include <stdint.h>
#include <stdbool.h>

#include "lcd.h"
#include "util.h"

#include <avr/io.h>

void lcd_init() {
	// Set up the pins connected to the LCD as outputs
	DDRD |= 1 << SCEPIN;
	DDRB |= 1 << RSTPIN;
	DDRB |= 1 << DCPIN;
	DDRB |= 1 << DINPIN;
	DDRF |= 1 << SCKPIN;

	PORTB &= ~(1 << RSTPIN);
	PORTD |= 1 << SCEPIN;
	PORTB |= 1 << RSTPIN;

	lcd_write(LCD_C, 0x21); // Enable LCD extended command set
	lcd_write(LCD_C, 0x80 | LCD_DEFAULT_CONTRAST); // Set LCD Vop (Contrast)
	lcd_write(LCD_C, 0x04);
	lcd_write(LCD_C, 0x13); // LCD bias mode 1:48

	lcd_write(LCD_C, 0x0C); // LCD in normal mode.
	lcd_write(LCD_C, 0x20); // Enable LCD basic command set
	lcd_write(LCD_C, 0x0C);
}

void lcd_write(uint8_t dc, uint8_t data) {
	// Set the DC pin based on the parameter 'dc'
	PORTB = (PORTB & (~(1 << DCPIN))) | ((dc & 1) << DCPIN);

	// Pull the SCE/SS pin low to signal the LCD we have data
	PORTD &= ~(1 << SCEPIN);

	// Write the byte of data using "bit bashing"
	for (int8_t i = 7; i >= 0; i--) {
		PORTF &= ~(1 << SCKPIN);
		PORTB = (PORTB & (~(1 << DINPIN))) | (((data >> i) & 1) << DINPIN);
		PORTF |= 1 << SCKPIN;
	}

	// Pull SCE/SS high to signal the LCD we are done
	PORTD |= 1 << SCEPIN;
}

void lcd_position(uint8_t x, uint8_t y) {
	lcd_write(LCD_C, (0x40 | y));
	lcd_write(LCD_C, (0x80 | x));
}

void lcd_invert(bool invert) {
	if (invert) {
		lcd_write(LCD_C, 0x0d);
	} else {
		lcd_write(LCD_C, 0x0c);
	}

	HOOK_1(end_invert, invert);
}

void lcd_contrast(uint8_t contrast) {
	if (contrast != LCD_LOW_CONTRAST && contrast != LCD_DEFAULT_CONTRAST && contrast != LCD_HIGH_CONTRAST) {
		return;
	}

	lcd_write(LCD_C, 0x21);
	lcd_write(LCD_C, 0x80 | contrast);
	lcd_write(LCD_C, 0x20);

	HOOK_1(end_contrast, contrast);
}
