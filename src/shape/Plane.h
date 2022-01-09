#pragma once
#include "math/Vec3.h"
#include "Triangle.h"
#include "Shape.h"



class Plane : public Shape {
public:
	Triangle t1;
	Triangle t2;
	Vec3 Vertices[4];

	Plane( Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) : 
		t1(v0, v1, v2),
		t2(v1, v3, v2),
		Vertices{v0, v1, v2, v3}
	{}

	bool RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction) {
		if(t1.RayCastTest(ray, hitPos, direction)) return true;
		if(t2.RayCastTest(ray, hitPos, direction)) return true;

		return false;
	}

	virtual ShapeType Type() override { return ShapeType::Plane; }
};