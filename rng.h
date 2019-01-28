#ifndef _RNG_H_
#define _RNG_H_

#include <stdint.h>

#include "util.h"

PACKED_STRUCT(tagRAND_STATE) {
	uint32_t x;
};

typedef struct tagRAND_STATE RAND_STATE, *PRAND_STATE;

void rand_seed(PRAND_STATE state, uint32_t seed);
uint32_t rand_next(PRAND_STATE state);
double rand_next_double(PRAND_STATE state);

#endif
