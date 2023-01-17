#pragma once
#include <math/Ray.h>
#include "../Color/Color.h"
#include "../HitInfo.h"
#include "../Shading/BsdfSample.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct SurfaceData
{
	glm::dvec3 normal;
	glm::dvec3 tangent;
	glm::dvec3 bitangent;
	glm::dvec3 position;
	glm::mat3 worldToTangent;
	glm::vec2 uv;
};

struct Material 
{
public:
	virtual bool scatter(const Ray3f& ray, const glm::dvec3& wi, const SurfaceData& surface, /*HitInfo & hitInfo,*/ Color& attenuation/*, Ray3f& scattered*/) const = 0;
	virtual glm::vec3 Albedo(const SurfaceData & surface) const = 0;
	virtual bool sampleBsdf(const SurfaceData & surface, const Ray3f & ray, BsdfSample & bsdfSample) const = 0;
};
