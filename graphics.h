#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdint.h>
#include <stdbool.h>

#include "util.h"

PACKED_STRUCT(tagPOINT) {
	int8_t x;
	int8_t y;
};

typedef struct tagPOINT POINT, *PPOINT;

#define POINT_NULL ((POINT) {0, 0})

void graphics_draw_line(POINT p1, POINT p2, bool filled);
void graphics_draw_rect(POINT p1, POINT p2, bool filled);
void graphics_draw_microdigit(POINT p, uint8_t digit);
void graphics_draw_char(POINT p, char c, bool filled);
void graphics_draw_string(POINT p, char* str, bool filled);
void graphics_draw_dot(POINT p, bool filled);
void graphics_draw_box(POINT p1, POINT p2, bool filled);

#endif
