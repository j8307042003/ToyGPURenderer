#pragma once
#include "IRandom.h"
#include <stdlib.h>

class SysRandom : public IRandom
{
public:
	virtual float random() override
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return r;
	}


	static float Random() 
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return r;		
	}

};
