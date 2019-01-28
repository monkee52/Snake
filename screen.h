#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdint.h>

#include "lcd.h"

extern uint8_t screen_buffer[LCD_BUFFER_SIZE];

void screen_set_pixel(uint8_t x, uint8_t y, uint8_t value);
void screen_clear();
void screen_update();

#endif
