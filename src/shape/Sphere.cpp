#include "Sphere.h"
#include "math/Vec3.h"
#include <math.h>
#include <iostream>

bool Sphere::RayCastTest(const Ray * ray, Vec3 & hitPos, Vec3 & direction) {
	const Vec3 diff = position - ray->origin;
	const Vec3 dir  = diff.normalized(); 

	const float lenDiff = diff.length();
	// if(ray->dir.z > 0.5)

	float angle = Vec3::Angle(ray->dir, dir);
	// std::cout << "angle " << angle << "  " << std::endl;
	if (abs(angle) * (180.0/3.141592653589793238463) > 90) return false;
	float normalLen = abs(tan(angle) * diff.length());
	// std::cout << "len " << normalLen << "  " << diff.length() << std::endl;
	if (normalLen > radius)
		return false;

	const float scale = (lenDiff - radius) / lenDiff;

	const float h = (1 / (cos(angle))) * lenDiff;
	hitPos = (scale * h) * ray->dir + ray->origin;

	const Vec3 normal = (hitPos - position).normalized();
	direction = Vec3::Reflect(ray->dir, normal); 

	return true;
}
