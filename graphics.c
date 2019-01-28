// Code based off cab202_teensy library

#include <stdint.h>
#include <stdbool.h>

#include "lcd.h"
#include "screen.h"
#include "graphics.h"

#include <avr/pgmspace.h>

// 010x08 MSBs make up the segment data for the microdigits
// 256x24 LSBs make up the bitmap data for the characters
static const uint32_t FONT[] PROGMEM = {
	0x7e000000, 0xb0257520, 0x6d275720, 0x79057720, 0x33027720, 0x5b277270, 0x5f227270, 0x70002000,
	0x7fffdfff, 0x7b075700, 0x00f8a8ff, 0x00031660, 0x00252720, 0x00232260, 0x00235120, 0x00275720,
	0x00467640, 0x00137310, 0x00272720, 0x00555050, 0x007dd550, 0x00365360, 0x00000070, 0x00272727,
	0x00272220, 0x00222720, 0x0002f200, 0x0004f400, 0x00004700, 0x00057500, 0x00027700, 0x00077200,
	0x00000000, 0x00222020, 0x00550000, 0x00575750, 0x00236362, 0x00412410, 0x00253570, 0x00640000,
	0x00244420, 0x00422240, 0x00527250, 0x00027200, 0x00000064, 0x00007000, 0x00000020, 0x00112440,
	0x00355560, 0x00262270, 0x00612470, 0x00612160, 0x00157110, 0x00746160, 0x00246520, 0x00713220,
	0x00252520, 0x00253120, 0x00002020, 0x00002064, 0x00124210, 0x00007070, 0x00421240, 0x00612020,
	0x00755470, 0x00257550, 0x00656560, 0x00344430, 0x00655560, 0x00746470, 0x00746440, 0x00345530,
	0x00557550, 0x00722270, 0x00111520, 0x00556550, 0x00444470, 0x00577550, 0x00575550, 0x00255520,
	0x00656440, 0x00255730, 0x00656550, 0x00347160, 0x00722220, 0x00555570, 0x00555520, 0x00557750,
	0x00552550, 0x00552220, 0x00712470, 0x00644460, 0x00442110, 0x00622260, 0x00250000, 0x0000000f,
	0x00620000, 0x00003570, 0x00446560, 0x00003430, 0x00113530, 0x00007630, 0x00127220, 0x00007517,
	0x00446550, 0x00202220, 0x00202226, 0x00445650, 0x00222220, 0x00007750, 0x00006550, 0x00002520,
	0x00006564, 0x00003531, 0x00006440, 0x00003260, 0x00027230, 0x00005570, 0x00005520, 0x00005770,
	0x00005250, 0x00005524, 0x00006230, 0x00326230, 0x00222220, 0x00623260, 0x005a0000, 0x00025700,
	0x00344724, 0x00505530, 0x00127630, 0x00253570, 0x00503570, 0x00423570, 0x00203570, 0x00074726,
	0x00257630, 0x00507630, 0x00427630, 0x00502220, 0x00250220, 0x00420220, 0x00525750, 0x00225750,
	0x00127670, 0x00003760, 0x00367670, 0x00252520, 0x00502520, 0x00422520, 0x00250570, 0x00425570,
	0x00505524, 0x00525520, 0x00505570, 0x00274720, 0x00127270, 0x00572720, 0x00066550, 0x00323260,
	0x00123570, 0x00120220, 0x00127570, 0x00120570, 0x00707550, 0x00705750, 0x00357070, 0x00252070,
	0x00202430, 0x00074400, 0x000e2200, 0x00452530, 0x00452710, 0x00202220, 0x0005a500, 0x000a5a00,
	0x00414141, 0x005a5a5a, 0x00bebebe, 0x00222222, 0x0022e222, 0x002e2e22, 0x0055d555, 0x0000f555,
	0x000e2e22, 0x005d1d55, 0x00555555, 0x000f1d55, 0x005d1f00, 0x0055f000, 0x002e2e00, 0x0000e222,
	0x00223000, 0x0022f000, 0x0000f222, 0x00223222, 0x0000f000, 0x0022f222, 0x00232322, 0x00555555,
	0x00554700, 0x00074555, 0x005d0f00, 0x000f0d55, 0x00554555, 0x000f0f00, 0x005d0d55, 0x002f0f00,
	0x0055f000, 0x000f0f22, 0x0000f555, 0x00557000, 0x00232300, 0x00032322, 0x00007555, 0x0055d555,
	0x002f0f22, 0x0022e000, 0x00003222, 0x00ffffff, 0x00000fff, 0x00cccccc, 0x00333333, 0x00fff000,
	0x00007670, 0x00256564, 0x00754440, 0x00755550, 0x00742470, 0x00003520, 0x00005574, 0x00016220,
	0x00725270, 0x00257520, 0x00025550, 0x00342520, 0x00007570, 0x00275720, 0x00347430, 0x00255550,
	0x00707070, 0x00272070, 0x00616070, 0x00343070, 0x00012222, 0x00222240, 0x00207020, 0x0005a5a0,
	0x00252000, 0x00027200, 0x00002000, 0x00322620, 0x00755000, 0x00624600, 0x00006600, 0x00000000
};

void graphics_draw_line(POINT p1, POINT p2, bool filled) {
	if (p1.x == p2.x) {
		// Draw vertical line
		for (int i = p1.y; (p2.y > p1.y) ? i <= p2.y : i >= p2.y; (p2.y > p1.y) ? i++ : i--) {
			screen_set_pixel(p1.x, i, filled);
		}
	} else if (p1.y == p2.y) {
		// Draw horizontal line
		for (int i = p1.x; (p2.x > p1.x) ? i <= p2.x : i >= p2.x; (p2.x > p1.x) ? i++ : i--) {
			screen_set_pixel(i, p1.y, filled);
		}
	} else {
		// Get Bresenhaming...
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		float err = 0.0;
		float derr = ABS(dy / dx);

		for (int x = p1.x, y = p1.y; (dx > 0) ? x <= p2.x : x >= p2.x; (dx > 0) ? x++ : x--) {
			screen_set_pixel(x, y, filled);
			err += derr;

			while (err >= 0.5 && ((dy > 0) ? y <= p2.y : y >= p2.y)) {
				screen_set_pixel(x, y, filled);
				y += (dy > 0) - (dy < 0);
				err -= 1.0;
			}
		}
	}
}

void graphics_draw_rect(POINT p1, POINT p2, bool filled) {
	int8_t top = MIN(p1.y, p2.y);
	int8_t left = MIN(p1.x, p2.x);
	int8_t width = MAX(p1.x, p2.x) - left;
	int8_t height = MAX(p1.y, p2.y) - top;

	if (width < 0 || height < 0) {
		return;
	}

	if (top >= LCD_Y || left >= LCD_X) {
		return;
	}

	for (int8_t y = 0; y < height; y++) {
		graphics_draw_line((POINT) { p1.x, p1.y + y }, (POINT) { p2.x, p1.y + y }, filled);
	}
}

void graphics_draw_box(POINT p1, POINT p2, bool filled) {
	int8_t top = MIN(p1.y, p2.y);
	int8_t left = MIN(p1.x, p2.x);
	int8_t width = MAX(p1.x, p2.x) - left;
	int8_t height = MAX(p1.y, p2.y) - top;

	if (width < 0 || height < 0) {
		return;
	}

	if (top >= LCD_Y || left >= LCD_X) {
		return;
	}

	graphics_draw_line((POINT) { left, top }, (POINT) { left + width, top }, filled);
	graphics_draw_line((POINT) { left, top + height }, (POINT) { left + width, top + height }, filled);
	graphics_draw_line((POINT) { left, top }, (POINT) { left, top + height }, filled);
	graphics_draw_line((POINT) { left + width, top }, (POINT) { left + width, top + height }, filled);
}

void graphics_draw_microdigit(POINT p, uint8_t digit) {
	if (digit > 9) {
		return;
	}

	uint8_t data = pgm_read_dword(&(FONT[digit])) >> 24;

	graphics_draw_rect(p, (POINT) { p.x + 2, p.y + 4 }, false);

	// 'A' segment
	if ((data >> 6) & 1) {
		screen_set_pixel(p.x + 0, p.y + 0, 1);
		screen_set_pixel(p.x + 1, p.y + 0, 1);
		screen_set_pixel(p.x + 2, p.y + 0, 1);
	}

	// 'B' segment
	if ((data >> 5) & 1) {
		screen_set_pixel(p.x + 2, p.y + 0, 1);
		screen_set_pixel(p.x + 2, p.y + 1, 1);
		screen_set_pixel(p.x + 2, p.y + 2, 1);
	}

	// 'C' segment
	if ((data >> 4) & 1) {
		screen_set_pixel(p.x + 2, p.y + 2, 1);
		screen_set_pixel(p.x + 2, p.y + 3, 1);
		screen_set_pixel(p.x + 2, p.y + 4, 1);
	}

	// 'D' segment
	if ((data >> 3) & 1) {
		screen_set_pixel(p.x + 0, p.y + 4, 1);
		screen_set_pixel(p.x + 1, p.y + 4, 1);
		screen_set_pixel(p.x + 2, p.y + 4, 1);
	}

	// 'E' segment
	if ((data >> 2) & 1) {
		screen_set_pixel(p.x + 0, p.y + 2, 1);
		screen_set_pixel(p.x + 0, p.y + 3, 1);
		screen_set_pixel(p.x + 0, p.y + 4, 1);
	}

	// 'F' segment
	if ((data >> 1) & 1) {
		screen_set_pixel(p.x + 0, p.y + 0, 1);
		screen_set_pixel(p.x + 0, p.y + 1, 1);
		screen_set_pixel(p.x + 0, p.y + 2, 1);
	}

	// 'G' segment
	if ((data >> 0) & 1) {
		screen_set_pixel(p.x + 0, p.y + 2, 1);
		screen_set_pixel(p.x + 1, p.y + 2, 1);
		screen_set_pixel(p.x + 2, p.y + 2, 1);
	}

	// 'H' segment
	if ((data >> 7) & 1) {
		screen_set_pixel(p.x + 1, p.y + 1, 1);
	}
}

void graphics_draw_char(POINT p, char c, bool filled) {
	uint32_t data = pgm_read_dword(&(FONT[(uint8_t)c]));

	if (!filled) {
		data = ~data;
	}

	for (int8_t i = 0; i < 24; i++) {
		screen_set_pixel(p.x + (i & 3), p.y + (i >> 2), (data >> (23 - i)) & 1);
	}
}

void graphics_draw_string(POINT p, char* str, bool filled) {
	uint16_t i = 0;

	while (*str != 0) {
		graphics_draw_char((POINT) { p.x + i * 4, p.y }, *str, filled);

		str++;
		i++;
	}
}

void graphics_draw_dot(POINT p, bool filled) {
	screen_set_pixel(p.x, p.y, filled);
}
