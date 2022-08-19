#pragma once
#include "Material.h"
#include <glm/vec3.hpp>
#include "../Texture/Texture.h"
#include <functional>
#include "../Shading/Model/Disney.h"

enum class PBRModel
{
	Disney
};





inline bool DisneyBRDF(const DisneyBRDFParam & param, const Ray3f & ray, const glm::vec3 & wi, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered);


class PBMaterial : public Material
{
public:
	PBMaterial();
    virtual bool scatter(const Ray3f & ray, const glm::dvec3& wi, const SurfaceData & surface, /*HitInfo & hitInfo,*/ Color & attenuation/*, Ray3f& scattered*/) const override;
	virtual glm::vec3 Albedo(const SurfaceData & surface) const override;
	virtual bool sampleBsdf(const SurfaceData & surface, const Ray3f & ray, BsdfSample & bsdfSample) const override;

private:
	DisneyBRDFParam MakeParam(const SurfaceData & surface) const;

public:
	PBRModel pbrmodel;

	float ior = 1.4f;
	float metallic = 0.0f;
	float roughness = 0.9f;
	float specularScale = 1.0f;
	glm::vec3 color = glm::vec3(1.0);
	glm::vec3 emission = glm::vec3(0.0);
	Texture * albedo_texture = nullptr;
	Texture * metallic_texture = nullptr;


	std::function<DisneyBRDFParam(const SurfaceData & surface, const PBMaterial &)> shader;
};
