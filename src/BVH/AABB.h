#pragma once
#ifndef AABB_H
#define AABB_H

#include "math/Ray.h"
#include "math/Vec3.h"
#include "shape/Plane.h"
#include "shape/Sphere.h"
#include "shape/Triangle.h"

class AABB {
public:
	Vec3 min;
	Vec3 max;

	AABB() : min({0,0,0}), max({0,0,0}) {}
	AABB(Vec3 min, Vec3 max) : min(min), max(max) {}

	bool RayIntersect(const Ray & ray);
};

static AABB aabbZero({0,0,0}, {0,0,0});


inline AABB merge_aabb(const AABB & a, const AABB & b){
	return AABB(min(a.min, b.min), max(a.max, b.max));	
}

inline Vec3 center(const AABB & a){
	return (a.min + a.max) / 2;
}

inline AABB make_aabb(const Sphere & s) {
	return AABB(s.position - s.radius, s.position + s.radius);
}


inline AABB make_aabb(const Triangle & t) {
	return AABB(
		{
			std::min(std::min(t.Vertices[0].x, t.Vertices[1].x), t.Vertices[2].x),
			std::min(std::min(t.Vertices[0].y, t.Vertices[1].y), t.Vertices[2].y),
			std::min(std::min(t.Vertices[0].z, t.Vertices[1].z), t.Vertices[2].z)
		},

		{
			std::max(std::max(t.Vertices[0].x, t.Vertices[1].x), t.Vertices[2].x),
			std::max(std::max(t.Vertices[0].y, t.Vertices[1].y), t.Vertices[2].y),
			std::max(std::max(t.Vertices[0].z, t.Vertices[1].z), t.Vertices[2].z)
		}		

	);
}

inline AABB make_aabb(const Plane & p) {
	return AABB(
		{
			std::min(std::min(std::min(p.Vertices[0].x, p.Vertices[1].x), p.Vertices[2].x), p.Vertices[3].x),
			std::min(std::min(std::min(p.Vertices[0].y, p.Vertices[1].y), p.Vertices[2].y), p.Vertices[3].y),
			std::min(std::min(std::min(p.Vertices[0].z, p.Vertices[1].z), p.Vertices[2].z), p.Vertices[3].z)
		},

		{
			std::max(std::max(std::max(p.Vertices[0].x, p.Vertices[1].x), p.Vertices[2].x), p.Vertices[3].x),
			std::max(std::max(std::max(p.Vertices[0].y, p.Vertices[1].y), p.Vertices[2].y), p.Vertices[3].y),
			std::max(std::max(std::max(p.Vertices[0].z, p.Vertices[1].z), p.Vertices[2].z), p.Vertices[3].z)
		}		
	);
}

#endif

