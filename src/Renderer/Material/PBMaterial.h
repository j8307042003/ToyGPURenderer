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
	virtual glm::vec3 Albedo() const override {return color;}
public:
	PBRModel pbrmodel;
	float ior = 1.4f;
	float metallic;
	float roughness = 0.9f;
	glm::vec3 color;
	glm::vec3 emission;
};



