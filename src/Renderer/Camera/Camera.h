#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <glm/geometric.hpp>
#include <math/Ray.h>
#include <glm/gtc/quaternion.hpp>

using vec2 = glm::vec2;
using vec3 = glm::vec3;

// Standard Camera Data Model
struct CameraData
{
	float film;
	float lens;
};

inline CameraData DefaultCameraData()
{
    return { 0.036f, 0.05f };
}

inline Ray3f SampleCamRay(const CameraData & cam, const vec3 & pos, const vec3 & direction, const vec2 & filmRes, const vec2 & pixelPos)
{
	vec3 uv(pixelPos.x / filmRes.x, pixelPos.y / filmRes.y, 0);
	auto e = vec3(0);
  	auto q = vec3{
      			// film * (0.5f - uv.x), film * (uv.y - 0.5f), lens
      			cam.film * (0.5f - uv.x), cam.film / (filmRes.x / filmRes.y) * (uv.y - 0.5f), cam.lens

      		 };
  	auto q1  = -q;
  	auto d   = glm::normalize(q1 - e);
    d.y = -d.y;
  	// d.z = -d.z;
  	// d.y = -d.y;
  	// d = d * 0.5f + 0.5f;
  	// auto ray = Ray{transform.TransformPoint(e), transform.TransformDir(d)};
    d = glm::quatLookAt(direction, glm::vec3(0.0, 1.0, 0.0)) * d;
  	auto ray = Ray3f{pos, (d)};
  	return ray;
}