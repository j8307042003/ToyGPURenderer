#pragma once
#ifndef _SPHERE_H_
#define _SPHERE_H_
#include "math/Vec3.h"
#include "Shape.h"
#include <glm/gtx/vector_angle.hpp>



class Sphere : public Shape {
public:
	Vec3 position;
	float radius;

	Sphere(Vec3 position, float radius) : position(position), radius(radius) {}

	bool RayCastTest(const Ray* ray, Vec3 & hitPos, Vec3 & direction);
	virtual ShapeType Type() override {return ShapeType::Sphere;}
};

inline bool IntersectSphere(const glm::vec3& position, float radius, const Ray3f& ray, glm::vec3& hit, glm::vec3& direction)
{
	const glm::vec3 diff = position - ray.origin;
	const glm::vec3 dir  = glm::normalize(diff); 

	const float lenDiff = glm::length(diff);

	float angle = glm::angle(ray.direction, dir);
	if (abs(angle) * (180.0/3.141592653589793238463) > 90) return false;
	float normalLen = abs(tan(angle) *  glm::length(diff));
	if (normalLen > radius)
		return false;

	const float scale = (lenDiff - radius) / lenDiff;

	const float h = (1 / (cos(angle))) * lenDiff;
	hit = (scale * h) * ray.direction + ray.origin;

	const glm::vec3 normal = glm::normalize(hit - position);
	direction = glm::reflect(ray.direction, normal); 

	return true;	
}

#endif