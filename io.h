#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>
#include <stdbool.h>

#define BTN_DPAD_LEFT 0
#define BTN_DPAD_RIGHT 1
#define BTN_DPAD_UP 2
#define BTN_DPAD_DOWN 3
#define BTN_DPAD_CENTER 4

#define BTN_LEFT 5
#define BTN_RIGHT 6

#define BTN_NONE 255

#define WHEEL_LEFT 0
#define WHEEL_RIGHT 1

#define LED_LEFT 0
#define LED_RIGHT 1
#define LED_TEENSY 2
#define LED_LCD 3

void io_set_led(uint8_t led, bool on);
bool io_get_btn(uint8_t btn);
bool io_get_btn_block(uint8_t btn);
uint16_t io_get_wheel(uint8_t wheel);
uint8_t io_get_joystick();
uint8_t io_get_joystick_block();

void io_init();

double systime();

#endif
