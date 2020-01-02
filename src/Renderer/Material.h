#pragma once
#include "math/Vec3.h"




struct material {
	material(Vec3 color, Vec3 emission) : color(color), emission(emission) {}
	Vec3 color = {0, 0, 0};	
	Vec3 emission = {0, 0, 0};
};