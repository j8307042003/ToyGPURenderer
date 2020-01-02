#pragma once
#include "math/Vec3.h"
#include "Shape.h"

class Sphere : public Shape {
public:
	Vec3 position;
	float radius;

	Sphere(Vec3 position, float radius) : position(position), radius(radius) {}

	bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction);

};