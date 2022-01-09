#pragma once

#include "math/Vec3.h"
#include "Shape.h"

class Triangle : public Shape {
public:
	// Vec3 position;
	Vec3 Vertices[3];

	Triangle(Vec3 v1, Vec3 v2, Vec3 v3 );

	const Vec3 & normal;

	bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction);
	virtual ShapeType Type() override {return ShapeType::Triangle;}


private:
	Vec3 _normal;

	Vec3 make_normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3);
};