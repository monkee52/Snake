#include <stdint.h>
#include <stdbool.h>

#include "slip.h"

#include <usb_serial.h>

void slip_send(uint8_t* data, uint32_t len) {
	uint32_t sent = 0;

	usb_serial_putchar(SLIP_END);
	sent++;

	while (len--) {
		switch (*data) {
		case SLIP_END:
			usb_serial_putchar(SLIP_ESC);
			sent++;
			usb_serial_putchar(SLIP_ESC_END);
			sent++;
			break;
		case SLIP_ESC:
			usb_serial_putchar(SLIP_ESC);
			sent++;
			usb_serial_putchar(SLIP_ESC_ESC);
			sent++;
			break;
		default:
			usb_serial_putchar(*data);
			sent++;
		}

		data++;
	}

	usb_serial_putchar(SLIP_END);
	sent++;

	while (sent % 64 != 0) {
		usb_serial_putchar(0x00);
		sent++;
	}

	usb_serial_flush_output();
}

uint32_t slip_recv(uint8_t* buffer, uint32_t len) {
	uint8_t c;
	uint32_t received = 0;

	while (true) {
		c = usb_serial_getchar();

		switch (c) {
		case SLIP_END:
			if (received) {
				return received;
			} else {
				break;
			}
		case SLIP_ESC:
			c = usb_serial_getchar();

			switch (c) {
			case SLIP_ESC_END:
				c = SLIP_END;
				break;
			case SLIP_ESC_ESC:
				c = SLIP_ESC;
				break;
			}
		default:
			if (received < len) {
				buffer[received++] = c;
			}
		}
	}
}
