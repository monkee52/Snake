#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "screen.h"
#include "graphics.h"
#include "rng.h"

#include <util/delay.h>

Hook1 hook_end_update = NULL;
Hook1 hook_end_invert = NULL;
Hook1 hook_end_contrast = NULL;

uint8_t hk_screen_flags;

#pragma region Utils

#ifdef DEBUG
#include <usb_serial.h>
#include "stcp.h"

void hk_update(const char* fn, uint8_t p1) {
	if (!usb_configured()) {
		return;
	}

	if (strcmp(fn, "lcd_invert") == 0) {
		hk_screen_flags &= ~1;

		if (p1) {
			hk_screen_flags |= 1;
		}
	} else if (strcmp(fn, "lcd_contrast") == 0) {
		hk_screen_flags &= ~6;

		if (p1 == LCD_LOW_CONTRAST) {
			hk_screen_flags |= 2;
		} else if (p1 == LCD_DEFAULT_CONTRAST) {
			hk_screen_flags |= 4;
		} else if (p1 == LCD_HIGH_CONTRAST) {
			hk_screen_flags |= 6;
		}
	}

	PSTCP_PACKET_SCREEN packet = stcp_create_screen(hk_screen_flags, screen_buffer);

	if (packet != NULL) {
		stcp_send((PSTCP_HEADER)packet);

		free(packet);
	}
}
#endif

void setup() {
	io_init();
	lcd_init();

#ifdef DEBUG
	usb_init();

	HOOK_DEF(end_update, hk_update);
	HOOK_DEF(end_contrast, hk_update);
	HOOK_DEF(end_invert, hk_update);
#endif
}

#pragma endregion

#define SNAKE_STATE_NOTSTARTED 0
#define SNAKE_STATE_RUNNING 1
#define SNAKE_STATE_GAMEOVER 2

#define SNAKE_X 27
#define SNAKE_Y 13

int8_t actual_mod(int8_t a, int8_t b) {
	int8_t r = a % b;

	return r < 0 ? r + b : r;
}

struct tagSNAKE_STATE {
	uint8_t direction;
	uint16_t score;
	uint8_t lives;
	bool walls;
	uint8_t state;
	uint16_t length;
	RAND_STATE rng;
	POINT food;
	PPOINT blocks;
};

typedef struct tagSNAKE_STATE SNAKE_STATE, *PSNAKE_STATE;

void snake_show_intro() {
	screen_clear();

	graphics_draw_string((POINT) { 22, 18 }, "Ayden Hull", true);
	graphics_draw_string((POINT) { 26, 24 }, "n9749675", true);

	screen_update();

	_delay_ms(2000);

	lcd_invert(true);
	_delay_ms(500);
	lcd_invert(false);
	_delay_ms(500);
	lcd_invert(true);
	_delay_ms(500);
	lcd_invert(false);
}

bool snake_intersects(PSNAKE_STATE state, POINT p) {
	if (state->walls && (((p.x == 6 || p.x == 20) && p.y >= 0 && p.y < 5) || (p.x == 13 && p.y >= 8 && p.y < 13))) {
		return true;
	}

	for (uint8_t i = 0; i < state->length; i++) {
		if (state->blocks[i].x == p.x && state->blocks[i].y == p.y) {
			return true;
		}
	}

	return false;
}

void snake_init(PSNAKE_STATE state) {
	state->direction = BTN_NONE;
	state->score = 0;
	state->lives = 3;
	state->walls = false;

	state->state = SNAKE_STATE_NOTSTARTED;

	state->length = 0;
	state->blocks = NULL;

	state->food.x = 0;
	state->food.y = 0;

	rand_seed(&state->rng, (uint32_t)(systime() * 1000.0f));
}

void snake_draw_layout(PSNAKE_STATE state) {
	// Score
	graphics_draw_string((POINT) { 0, 0 }, "SCORE", true);

	graphics_draw_microdigit((POINT) { 24, 0 }, (state->score / 1000) % 10);
	graphics_draw_microdigit((POINT) { 28, 0 }, (state->score / 100) % 10);
	graphics_draw_microdigit((POINT) { 32, 0 }, (state->score / 10) % 10);
	graphics_draw_microdigit((POINT) { 36, 0 }, state->score % 10);

	// Lives
	graphics_draw_string((POINT) { 46, 0 }, "LIVES", true);
	graphics_draw_microdigit((POINT) { 70, 0 }, (state->lives / 100) % 10);
	graphics_draw_microdigit((POINT) { 74, 0 }, (state->lives / 10) % 10);
	graphics_draw_microdigit((POINT) { 78, 0 }, state->lives % 10);

	if (state->state == SNAKE_STATE_GAMEOVER) {
		graphics_draw_line((POINT) { 0, 6 }, (POINT) { LCD_X - 1, 6 }, true);

		graphics_draw_string((POINT) { 0,  8 }, "GAME OVER!", true);
		graphics_draw_string((POINT) { 0, 14 }, "POWER CYCLE TO TRY", true);
		graphics_draw_string((POINT) { 0, 20 }, "AGAIN.", true);
	} else {
		// Food
		graphics_draw_dot((POINT) {
			state->food.x * 3 + 1 + 1,
			state->food.y * 3 + 7 + 0
		}, true);

		graphics_draw_dot((POINT) {
			state->food.x * 3 + 1 + 0,
			state->food.y * 3 + 7 + 1
		}, true);

		graphics_draw_dot((POINT) {
			state->food.x * 3 + 1 + 2,
			state->food.y * 3 + 7 + 1
		}, true);

		graphics_draw_dot((POINT) {
			state->food.x * 3 + 1 + 1,
			state->food.y * 3 + 7 + 2
		}, true);

		// Box
		graphics_draw_box((POINT) { 0, 6 }, (POINT) { 82, 46 }, true);

		for (uint8_t i = 0; i < state->length; i++) {
			graphics_draw_rect((POINT) {
				state->blocks[i].x * 3 + 1,
				state->blocks[i].y * 3 + 7
			}, (POINT) {
				state->blocks[i].x * 3 + 1 + 2,
				state->blocks[i].y * 3 + 7 + 3
			}, true);
		}

		if (state->walls) {
			graphics_draw_line((POINT) {  6 * 3 + 1 + 1, 0 * 3 + 7 }, (POINT) {  6 * 3 + 1 + 1,  5 * 3 + 7 }, true);
			graphics_draw_line((POINT) { 13 * 3 + 1 + 1, 8 * 3 + 7 }, (POINT) { 13 * 3 + 1 + 1, 13 * 3 + 7 }, true);
			graphics_draw_line((POINT) { 20 * 3 + 1 + 1, 0 * 3 + 7 }, (POINT) { 20 * 3 + 1 + 1,  5 * 3 + 7 }, true);
		}
	}
}

bool snake_tick(PSNAKE_STATE state) {
	if (state->walls && (((state->blocks[0].x == 6 || state->blocks[0].x == 20) && state->blocks[0].y >= 0 && state->blocks[0].y < 5) || (state->blocks[0].x == 13 && state->blocks[0].y >= 8 && state->blocks[0].y < 13))) {
		return true;
	}

	POINT head = state->blocks[0];

	switch (state->direction) {
	case BTN_DPAD_UP:
		head.y--;
		break;
	case BTN_DPAD_RIGHT:
		head.x++;
		break;
	case BTN_DPAD_DOWN:
		head.y++;
		break;
	case BTN_DPAD_LEFT:
		head.x--;
		break;
	default:
		break;
	}

	bool append = false;
	POINT appendBlock;

	if (head.x == state->food.x && head.y == state->food.y) {
		append = true;
		appendBlock = state->blocks[state->length - 1];

		state->score += state->walls ? 2 : 1;
	}

	for (uint8_t i = state->length - 1; i >= 1; i--) {
		state->blocks[i] = state->blocks[i - 1];
	}

	state->blocks[0] = head;

	if (append) {
		void* tmp = realloc(state->blocks, sizeof(POINT) * (state->length + 1));

		if (tmp != NULL) {
			state->blocks = (PPOINT)tmp;
			state->length++;

			state->blocks[state->length - 1] = appendBlock;
		}

		do {
			state->food.x = rand_next_double(&state->rng) * SNAKE_X;
			state->food.y = rand_next_double(&state->rng) * SNAKE_Y;
		} while (snake_intersects(state, state->food));
	}

	for (uint8_t i = 0; i < state->length; i++) {
		state->blocks[i].x = actual_mod(state->blocks[i].x, SNAKE_X);
		state->blocks[i].y = actual_mod(state->blocks[i].y, SNAKE_Y);

		if (i != 0 && state->blocks[i].x == state->blocks[0].x && state->blocks[i].y == state->blocks[0].y) {
			return true;
		}
	}

	return false;
}

void sleep(uint32_t ms) {
	double d = (double)ms / 1000.0f;
	double t = systime();

	while (systime() - t < d);
}

int main() {
	setup();

	SNAKE_STATE state;

	snake_init(&state);

	snake_show_intro();

	double ticks = 0.0f;

	while (true) {
		if (io_get_btn(BTN_LEFT)) {
			state.walls = false;
		}

		if (io_get_btn(BTN_RIGHT)) {
			state.walls = true;
		}

		uint8_t dir = io_get_joystick();

		if (state.state == SNAKE_STATE_NOTSTARTED) {
			if (state.blocks != NULL) {
				free(state.blocks);

				state.length = 0;
				state.blocks = NULL;
			}

			if (state.blocks == NULL) {
				state.length = 2;
				state.blocks = (PPOINT)malloc(sizeof(POINT) * state.length);

				state.blocks[0].x = 1;
				state.blocks[0].y = 0;
				state.blocks[1].x = 0;
				state.blocks[1].y = 0;
			}

			if (dir != BTN_NONE && dir != BTN_DPAD_CENTER && dir != BTN_DPAD_LEFT) {
				state.state = SNAKE_STATE_RUNNING;
				state.direction = dir;

				do {
					state.food.x = rand_next_double(&state.rng) * SNAKE_X;
					state.food.y = rand_next_double(&state.rng) * SNAKE_Y;
				} while (snake_intersects(&state, state.food));
			}
		} else if (state.state == SNAKE_STATE_RUNNING) {
			bool take_life = false;

			if (ticks > 1.0f) {
				take_life = take_life || snake_tick(&state);
				ticks = 0.0f;
			}

			if (dir != BTN_NONE && dir != BTN_DPAD_CENTER) {
				bool dir_invalid = (state.direction == BTN_DPAD_LEFT && dir == BTN_DPAD_RIGHT) || (state.direction == BTN_DPAD_RIGHT && dir == BTN_DPAD_LEFT) || (state.direction == BTN_DPAD_UP && dir == BTN_DPAD_DOWN) || (state.direction == BTN_DPAD_DOWN && dir == BTN_DPAD_UP);

				take_life = take_life || dir_invalid;
				
				if (!dir_invalid) {
					state.direction = dir;
				}
			}

			if (take_life) {
				if (state.lives) {
					state.state = SNAKE_STATE_NOTSTARTED;
					state.direction = BTN_NONE;
					state.lives--;
				} else {
					state.state = SNAKE_STATE_GAMEOVER;
				}
			}
		}

		screen_clear();

		snake_draw_layout(&state);

		screen_update();

		//sleep(1);

		ticks += (double)io_get_wheel(WHEEL_RIGHT) / 1023.0f;
	}

	// 27x13 = 351 bits / 44 bytes

	return 0;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
