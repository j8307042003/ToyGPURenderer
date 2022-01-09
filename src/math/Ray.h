#pragma once
#ifndef Ray_H
#define Ray_H
#include "glm/vec3.hpp"
#include "Vec3.h"

struct Ray3f {
	glm::vec3 origin;
	glm::vec3 direction;
};

class Ray {
public:
	Ray(){}
	Ray(Vec3 origin, Vec3 dir);

	Vec3 origin;
	Vec3 dir;

	
};

#endif