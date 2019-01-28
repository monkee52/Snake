// Code based off cab202_teensy library

#include <stdint.h>

#include "lcd.h"
#include "screen.h"
#include "util.h"

uint8_t screen_buffer[LCD_BUFFER_SIZE];

void screen_set_pixel(uint8_t x, uint8_t y, uint8_t value) {
	if (x < 0 || x > LCD_X || y < 0 || y > LCD_Y) {
		return;
	}

	uint16_t addr = ((uint16_t)y >> 3) * 84 + (uint16_t)x;
	uint8_t subrow = y & 7;

	screen_buffer[addr] &= ~(1 << subrow);
	screen_buffer[addr] |= (value == 0 ? 0 : 1) << subrow;
}

void screen_clear() {
	for (uint16_t i = 0; i < LCD_BUFFER_SIZE; i++) {
		screen_buffer[i] = 0;
	}
}

void screen_update() {
	lcd_position(0, 0);

	for (uint16_t i = 0; i < LCD_BUFFER_SIZE; i++) {
		lcd_write(LCD_D, screen_buffer[i]);
	}

	HOOK_1(end_update, 0);
}
