#ifndef _STCP_H_
#define _STCP_H_

#include <stdint.h>
#include <stdbool.h>

#include "lcd.h"
#include "util.h"

#define STCP_TYPE_NONE 0
#define STCP_TYPE_MESSAGE 1
#define STCP_TYPE_BREAKPOINT 2
#define STCP_TYPE_BREAKPOINT_RETURN 3
#define STCP_TYPE_SCREEN 4

#define STCP_FLAGS_INVERT 0x01

#define STCP_FLAGS_CL 0x02
#define STCP_FLAGS_CD 0x04
#define STCP_FLAGS_CH 0x06

#define STCP_FLAGS_CMASK 0x06

#define STCP_ERR_NONE 0
#define STCP_ERR_BUFFER_OVERFLOW -1
#define STCP_ERR_ENOMEM -2
#define STCP_ERR_BAD_LENGTH -3
#define STCP_ERR_BAD_CHECKSUM -4
#define STCP_ERR_BAD_PACKET -5

PACKED_STRUCT(tagSTCP_HEADER) {
	char id[4];
	uint8_t type;
	uint16_t length;
	uint32_t checksum;
};

typedef struct tagSTCP_HEADER STCP_HEADER, *PSTCP_HEADER;

PACKED_STRUCT(tagSTCP_PACKET_MESSAGE) {
	STCP_HEADER header;
	float time;
	char message[0];
};

typedef struct tagSTCP_PACKET_MESSAGE STCP_PACKET_MESSAGE, *PSTCP_PACKET_MESSAGE;

PACKED_STRUCT(tagSTCP_PACKET_BREAKPOINT) {
	STCP_HEADER header;
	float time;
	uint16_t line;
	char file[0];
};

typedef struct tagSTCP_PACKET_BREAKPOINT STCP_PACKET_BREAKPOINT, *PSTCP_PACKET_BREAKPOINT;

PACKED_STRUCT(tagSTCP_PACKET_BREAKPOINT_RETURN) {
	STCP_HEADER header;
};

typedef struct tagSTCP_PACKET_BREAKPOINT_RETURN STCP_PACKET_BREAKPOINT_RETURN, *PSTCP_PACKET_BREAKPOINT_RETURN;

PACKED_STRUCT(tagSTCP_PACKET_SCREEN) {
	STCP_HEADER header;
	uint8_t flags;
	uint8_t screen[LCD_BUFFER_SIZE];
};

typedef struct tagSTCP_PACKET_SCREEN STCP_PACKET_SCREEN, *PSTCP_PACKET_SCREEN;

PSTCP_PACKET_MESSAGE stcp_create_message(char* msg);
PSTCP_PACKET_BREAKPOINT stcp_create_breakpoint(uint16_t line, char* file);
PSTCP_PACKET_BREAKPOINT_RETURN stcp_create_breakpoint_return();
PSTCP_PACKET_SCREEN stcp_create_screen(uint8_t flags, uint8_t* screen);
void stcp_send(PSTCP_HEADER packet);
int8_t stcp_recv(PSTCP_HEADER* packet);

void stcp_util_message(char* msg);
void stcp_util_break_IMPL(uint16_t line, char* file);

#define stcp_util_break() stcp_util_break_IMPL(__LINE__, __FILE__)

#endif
