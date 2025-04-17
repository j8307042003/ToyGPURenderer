#pragma once
#include "Material.h"
#include <glm/vec3.hpp>
#include <functional>


class EmissionMaterial : public Material
{
public:
	EmissionMaterial();
    virtual bool scatter(const glm::dvec3& view, const glm::dvec3& wi, const SurfaceData & surface, /*HitInfo & hitInfo,*/ Color & attenuation/*, Ray3f& scattered*/) const override;
	virtual glm::vec3 Albedo(const SurfaceData & surface) const override;
	virtual glm::vec3 Emission(const SurfaceData& surface) const override { return emission; };
	virtual bool sampleBsdf(const SurfaceData & surface, const Ray3f & ray, BsdfSample & bsdfSample) const override;
	virtual bool hit(const SurfaceData & surface) const override;

public:
	glm::vec3 emission = glm::vec3(0.0);
};
