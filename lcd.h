#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include <stdbool.h>

#define LCD_X 84
#define LCD_Y 48
#define LCD_BUFFER_SIZE 504

// What pins did we connect D/C and RST to
#define DCPIN  5 //PORTB
#define RSTPIN 4 //PORTB

// What pins are the SPI lines on
#define DINPIN 6 // PORTB
#define SCKPIN 7 // PORTF
#define SCEPIN 7 // PORTD

// LCD Command and Data
#define LCD_C 0
#define LCD_D 1

// LCD Contrast levels, you may have to change these for your display
#define LCD_LOW_CONTRAST     0x2F
#define LCD_DEFAULT_CONTRAST 0x3F
#define LCD_HIGH_CONTRAST    0x4F

void lcd_init();
void lcd_write(uint8_t dc, uint8_t data);
void lcd_position(uint8_t x, uint8_t y);

void lcd_invert(bool invert);
void lcd_contrast(uint8_t contrast);

#endif
