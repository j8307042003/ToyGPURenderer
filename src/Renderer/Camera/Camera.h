#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <glm/geometric.hpp>
#include <math/Ray.h>
#include <glm/gtc/quaternion.hpp>
#include "../Random/SysRandom.h"
#include <memory>
#include <functional>
#include <typeindex>
#include <typeinfo>

using vec2 = glm::vec2;
using vec3 = glm::vec3;

using CameraSampleRayFunc = std::function<Ray3f(const vec3 & pos, const vec3 & direction, const vec2 & filmRes, const vec2 & pixelPos, vec3& transmittance, bool SimCam)>;

// Standard Camera Data Model
struct CameraData
{
	std::type_index dataType = std::type_index(typeid(int));
	std::shared_ptr<void> camData;
	CameraSampleRayFunc sampleRay;
};

struct DefaultCameraDataMode
{
	float film;
	float lens;
	float focal;
};

inline Ray3f SampleCamRay(const DefaultCameraDataMode& cam, const vec3 & pos, const vec3 & direction, const vec2 & filmRes, const vec2 & pixelPos, vec3& transmittance, bool SimCam = false)
{
	transmittance = glm::vec3(1.0f);
	auto w = direction;
	vec3 u = glm::normalize(glm::cross(glm::vec3(0.0, 1.0f, 0.0f), w));
	vec3 v = glm::normalize(glm::cross(w, u));
	vec3 uv(pixelPos.x / filmRes.x, pixelPos.y / filmRes.y, 0);

	vec3 horizontal = cam.focal * filmRes.x * u;
	vec3 vertical = cam.focal * filmRes.y * v;
	vec3 lower_left_corner = pos - horizontal/2.0f - vertical/2.0f - cam.focal*w;

	//lens_radius = aperture / 2;

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

	vec3 offset = SimCam ? cam.focal * u * (SysRandom::Random() - 0.5f) + cam.focal * v * (SysRandom::Random() - 0.5f) : vec3(0.0f, 0.0f, 0.0f);


	d = glm::quatLookAt(direction, glm::vec3(0.0, 1.0, 0.0)) * d;
	auto ray = Ray3f{pos+offset, (d)};
	return ray;
}

inline CameraData DefaultCameraData()
{
    auto data = std::make_shared<DefaultCameraDataMode>();
	data->film = 0.036f;
	data->lens = 0.05f;
	data->focal = 0.0f;

    CameraData camdata = {};
    camdata.dataType = std::type_index(typeid(DefaultCameraDataMode)),
    camdata.camData = data;
    camdata.sampleRay = [=](const vec3& pos, const vec3& direction, const vec2& filmRes, const vec2& pixelPos, vec3& transmittance, bool SimCam) {
			return SampleCamRay(*data, pos, direction, filmRes, pixelPos, transmittance, SimCam);
        };
    
    return camdata;
}
