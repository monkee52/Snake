#include <stdint.h>

#include "rng.h"

// Implementation of a linear congruential generator
// a, c based on MSVC++ parameters

void rand_seed(PRAND_STATE state, uint32_t seed) {
	state->x = seed;
}

uint32_t rand_next(PRAND_STATE state) {
	state->x = 0x343fd * state->x + 0x269ec3;

	return state->x;
}

double rand_next_double(PRAND_STATE state) {
	return (double)rand_next(state) / (double)0xffffffff;
}
