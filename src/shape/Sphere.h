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

inline bool IntersectSphere(const glm::dvec3& position, float radius, const Ray3f& ray, float t_min, float t_max, glm::dvec3& hit)
{

/*	
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
*/

/*
	glm::vec3 oc = ray.origin - position;
	float b = dot( oc, ray.direction );
	float c = dot( oc, oc ) - radius * radius;
	float h = b*b - c;
	if( h<0.0 ) return false;

  	float sqrt_h = sqrt(h);
	float t = -b - sqrt_h;

  	float dir = 1;
  	if (t < 0.0) {
    	t = -b + sqrt_h;
    	dir = -1;
  	}
	if (t < 0.0) return false;

 	hit = ray.direction * t + ray.origin;
	const glm::vec3 normal = glm::normalize(hit - position) * dir;
	direction = glm::reflect(ray.direction, normal); 		


	return true;
*/

    glm::dvec3 oc = ray.origin - position;
    auto a = glm::length2(ray.direction);
    auto half_b = glm::dot(oc, ray.direction);
    auto c = glm::length2(oc) - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    /*
    if (root < 0.01)
    {
        root = (-half_b + sqrtd) / a;
        if (root < 0.01)
        	return false;
    }
    */
    
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }
    

    hit = ray.origin + ray.direction * root;
    auto normal = glm::normalize(hit - position);

    return true;
}

#endif
