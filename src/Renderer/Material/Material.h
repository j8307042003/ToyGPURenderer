#pragma once
#include <math/Ray.h>
#include "../Color/Color.h"
#include "../HitInfo.h"
#include <glm/vec3.hpp>

struct SurfaceData
{
	glm::dvec3 normal;
    glm::dvec3 position;
};

struct Material 
{
public:
	virtual bool scatter(const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered) const = 0;
	virtual glm::vec3 Albedo() const = 0;
};
