#pragma once
#include "Material.h"
#include <glm/vec3.hpp>
#include "../Texture/Texture.h"
#include <functional>

enum class PBRModel
{
	Disney
};


struct DisneyBRDFParam
{
	float ior = 1.4f;
	float metallic;
	float roughness;
	glm::vec3 color;
	glm::vec3 emission;	
	glm::dvec3 position;
	glm::dvec3 normal;
};



inline bool DisneyBRDF(const DisneyBRDFParam & param, const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered);


class PBMaterial : public Material
{
public:
	PBMaterial();
    virtual bool scatter(const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered) const override;
	virtual glm::vec3 Albedo(const SurfaceData & surface) const override;

public:
	PBRModel pbrmodel;

	float ior = 1.4f;
	float metallic = 0.0f;
	float roughness = 0.9f;
	glm::vec3 color = glm::vec3(1.0);
	glm::vec3 emission = glm::vec3(0.0);
	Texture * albedo_texture = nullptr;
	Texture * metallic_texture = nullptr;


	std::function<DisneyBRDFParam(const SurfaceData & surface, const PBMaterial &)> shader;
};
