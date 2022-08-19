#pragma once
#include "IRandom.h"
#include <stdlib.h>
#include <stdint.h>

#define SYS_RANDOM

class SysRandom : public IRandom
{
public:
	virtual float random() override
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return r;
	}

#ifdef SYS_RANDOM
	static float Random() 
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return r;		
	}
#else
	typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

	static float Random()
	{
		static pcg32_random_t seed;

		//return static_cast <double>(pcg32_random_r(&seed)) / static_cast <double>(UINT32_MAX);
		auto i = pcg32_random_r(&seed);

		return ldexp(float(i), -32);
	}

	static uint32_t pcg32_random_r(pcg32_random_t* rng)
	{
	    uint64_t oldstate = rng->state;
	    // Advance internal state
	    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
	    // Calculate output function (XSH RR), uses old state for max ILP
	    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	    uint32_t rot = oldstate >> 59u;
	    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

#endif


};
