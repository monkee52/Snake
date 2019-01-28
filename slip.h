#ifndef _SLIP_H_
#define _SLIP_H_

#include <stdint.h>
#include <stdbool.h>

#define SLIP_END 0xc0
#define SLIP_ESC 0xdb
#define SLIP_ESC_END 0xdc
#define SLIP_ESC_ESC 0xdd

void slip_send(uint8_t* data, uint32_t len);
uint32_t slip_recv(uint8_t* buffer, uint32_t len);

#endif
