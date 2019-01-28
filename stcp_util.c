#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stcp.h"

void stcp_util_message(char* msg) {
	PSTCP_PACKET_MESSAGE packet = stcp_create_message(msg);

	if (packet != NULL) {
		stcp_send((PSTCP_HEADER)packet);

		free(packet);
	}
}

void stcp_util_break_IMPL(uint16_t line, char* file) {
	PSTCP_PACKET_BREAKPOINT packet = stcp_create_breakpoint(line, file);

	if (packet != NULL) {
		stcp_send((PSTCP_HEADER)packet);

		free(packet);
	}

	PSTCP_PACKET_BREAKPOINT_RETURN retPacket;
	int8_t err;

	while (true) {
		err = stcp_recv((PSTCP_HEADER*)&retPacket);

		if (err == STCP_ERR_NONE) {
			err = retPacket->header.type;

			free(retPacket);

			if ((uint8_t)err == STCP_TYPE_BREAKPOINT_RETURN) {
				break;
			}
		}
	}
}
