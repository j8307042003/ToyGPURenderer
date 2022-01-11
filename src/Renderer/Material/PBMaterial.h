#pragma once
#include "Material.h"
#include <glm/vec3.hpp>

enum class PBRModel
{
	Disney
};


class PBMaterial : public Material
{
public:
    virtual bool scatter(const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered) const override;

public:
	PBRModel pbrmodel;
	glm::vec3 color;
};



