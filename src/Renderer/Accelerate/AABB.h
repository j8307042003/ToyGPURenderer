#ifndef DAABB_H
#define DAABB_H

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <math/Ray.h>
#include <shape/Sphere.h>
#include <shape/Triangle.h>

struct dAABB
{
    dAABB() : max(glm::dvec3(std::numeric_limits<float>::lowest())), min(glm::dvec3(std::numeric_limits<float>::max())){}
    dAABB(glm::dvec3 min, glm::dvec3 max) : min(min), max(max){}
	glm::dvec3 min;
	glm::dvec3 max;
};

/*
	AABB() {
        float minNum = std::numeric_limits<float>::lowest();
        float maxNum = std::numeric_limits<float>::max();
        min = Vec3(maxNum, maxNum, maxNum);
        max = Vec3(minNum, minNum, minNum);		
	}
	AABB(Vec3 min, Vec3 max) : min(min), max(max) {}
*/

inline dAABB aabb_default() { return {glm::dvec3(std::numeric_limits<float>::max()), glm::dvec3(std::numeric_limits<float>::lowest())};}

inline glm::dvec3 aabb_diagonal(const dAABB & aabb){
	return aabb.max - aabb.min;
}

inline int aabb_maxExtent(const dAABB & aabb) {
	auto d = aabb_diagonal(aabb);
	if(d.x > d.y && d.x > d.z) return 0;
	else if (d.y > d.z) return 1;
	else return 2;
}


inline glm::vec2 fast_intersect_aabb(const dAABB & aabb, const glm::dvec3 & invdir, const glm::dvec3 & oxinvdir)
{
    glm::dvec3 f = aabb.max * invdir + oxinvdir;
    glm::dvec3 n = aabb.min * invdir + oxinvdir;
    glm::dvec3 tmax = max(f, n);
    glm::dvec3 tmin = min(f, n);
    float t1 = std::min(tmax.x, std::min(tmax.y, tmax.z));
    float t0 = std::max(tmin.x, std::max(tmin.y, tmin.z));
    return glm::vec2(t0, t1);
}

inline double min(glm::dvec3 a)
{
	return std::min(a.x, std::min(a.y, a.z));
}

inline double max(glm::dvec3 a)
{
	return std::max(a.x, std::max(a.y, a.z));
}

inline bool intersect_bbox(
	const Ray3f& ray, double ray_tmin, double ray_tmax, const glm::dvec3& ray_dinv, const dAABB& bbox) {
	auto it_min = (bbox.min - ray.origin) * ray_dinv;
	auto it_max = (bbox.max - ray.origin) * ray_dinv;
	auto tmin = min(it_min, it_max);
	auto tmax = max(it_min, it_max);
	auto t0 = std::max(max(tmin), ray_tmin);
	auto t1 = std::min(min(tmax), ray_tmax);
	t1 *= 1.00000024f;  // for double: 1.0000000000000004
	return t0 <= t1;
}


inline bool aabb_rayIntersect(const dAABB & aabb, const Ray3f & ray) {
	glm::dvec3 invDir = 1.0 / ray.direction;

	double tmin;
	double tmax;

	if ( invDir.x > 0 ){
		tmin = (aabb.min.x - ray.origin.x) * invDir.x;
		tmax = (aabb.max.x - ray.origin.x) * invDir.x;
	}
	else {
		tmin = (aabb.max.x - ray.origin.x) * invDir.x;
		tmax = (aabb.min.x - ray.origin.x) * invDir.x;
	}

	double t0y;
	double t1y;

	if (invDir.y > 0 ){
		t0y = (aabb.min.y - ray.origin.y) * invDir.y;
		t1y = (aabb.max.y - ray.origin.y) * invDir.y;
	}
	else {
		t1y = (aabb.min.y - ray.origin.y) * invDir.y;
		t0y = (aabb.max.y - ray.origin.y) * invDir.y;
	}


	if (tmin > t1y || t0y > tmax) return false;

	tmin = tmin > t0y ? tmin : t0y;
	tmax = tmax < t1y ? tmax : t1y;

	double t0z;
	double t1z;


	if (invDir.z > 0 ){
		t0z = (aabb.min.z - ray.origin.z) * invDir.z;
		t1z = (aabb.max.z - ray.origin.z) * invDir.z;
	}
	else {
		t1z = (aabb.min.z - ray.origin.z) * invDir.z;
		t0z = (aabb.max.z - ray.origin.z) * invDir.z;
	}

	if (tmin > t1z || tmax < t0z) return false;


	if (t0z > tmin) tmin = t0z;
	if (t1z < tmax) tmax = t1z;


	return true;
}


inline glm::dvec3 aabb_center(const dAABB & aabb) {return (aabb.max + aabb.min) / 2.0;}
inline glm::dvec3 aabb_offset(const dAABB & aabb, glm::dvec3 pos) {
	glm::dvec3 o = pos - aabb.min;
	if (aabb.max.x > aabb.min.x) o.x /= aabb.max.x - aabb.min.x;
	if (aabb.max.y > aabb.min.y) o.y /= aabb.max.y - aabb.min.y;
	if (aabb.max.z > aabb.min.z) o.z /= aabb.max.z - aabb.min.z;
	return o;
}

inline float aabb_surfaceArea(const dAABB & aabb) {
	auto size = aabb_diagonal(aabb);
	if (size.x < 0 || size.y < 0 || size.z < 0) return 0;
	return 2 * ( size.x * size.y + size.x * size.z + size.y * size.z );
}


inline dAABB merge_aabb(const dAABB & a, const dAABB & b){
	return { min(a.min, b.min), max(a.max, b.max) };
}

inline dAABB make_aabb(const glm::dvec3 & position, float radius) {
	return { position - glm::dvec3(radius), position + glm::dvec3(radius) };
}


//const glm::dvec3 & v0, const glm::dvec3 & v1, const glm::dvec3 & v2, const glm::dvec3 & normal
inline dAABB make_aabb(const glm::dvec3 & v0, const glm::dvec3 & v1, const glm::dvec3 & v2) {
	return {
		{
			std::min(std::min(v0.x, v1.x), v2.x),
			std::min(std::min(v0.y, v1.y), v2.y),
			std::min(std::min(v0.z, v1.z), v2.z)
		},

		{
			std::max(std::max(v0.x, v1.x), v2.x),
			std::max(std::max(v0.y, v1.y), v2.y),
			std::max(std::max(v0.z, v1.z), v2.z)
		}
	};
}

#endif
