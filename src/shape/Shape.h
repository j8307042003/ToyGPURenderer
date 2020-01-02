#pragma once

#include "math/Vec3.h"
#include "math/Ray.h"
#include "math/Transform.h"

class Shape{
public:
	Transform transform;
	Shape(){}

	virtual bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction){return false;}
};