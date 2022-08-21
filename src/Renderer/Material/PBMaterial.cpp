#include "PBMaterial.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../Random/SysRandom.h"
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include "../Common/common.h"
#include <glm/gtx/compatibility.hpp>
#include "Shader/Shader.h"
#include "../Shading/Model/Disney.h"

inline glm::dvec2 random_polar(float roughness) {
	float r1 = SysRandom::Random();
	float r2 = SysRandom::Random();

	// float theta1 = atan((roughness * sqrt(r1)) / (sqrt(1 - r1)));
	float theta1 = glm::acos(sqrt((1.0f - r1) / ((roughness * roughness - 1.0f) * r1 + 1.0f)));
	return glm::dvec2(theta1, 2 * 3.1415926f * r2);	
}

inline void make_coordinateSys(const glm::dvec3 & normal, glm::dvec3 & normal_t,  glm::dvec3 & normal_b) {
	if (abs(normal.x) > abs(normal.y))
		normal_t = glm::dvec3(normal.z, 0, -normal.x);
	else
		normal_t = glm::dvec3(0, -normal.z, normal.y);


	normal_b = glm::cross(normal, normal_t);		
}

inline glm::dvec3 uniformSampleHemisphere(float r1, float r2) 
{ 
    float sinTheta = glm::sqrt(1 - r1 * r1); 
    float phi = r2; 
    float x = sinTheta * glm::cos(phi); 
    float z = sinTheta * glm::sin(phi); 
    return glm::dvec3(x, r1, z); 
} 
 


inline glm::dvec4 make_GGXRandom(glm::dvec3 dir, glm::dvec2 polarSet) {
	glm::dvec3 Nb, Nt;
	make_coordinateSys(dir, Nb, Nt);

	float theta1 = glm::cos(polarSet.x);
	glm::dvec3 localHemiSphereDir = uniformSampleHemisphere(theta1, polarSet.y);

    return glm::dvec4( 
        localHemiSphereDir.x * Nb.x + localHemiSphereDir.y * dir.x + localHemiSphereDir.z * Nt.x, 
        localHemiSphereDir.x * Nb.y + localHemiSphereDir.y * dir.y + localHemiSphereDir.z * Nt.y, 
        localHemiSphereDir.x * Nb.z + localHemiSphereDir.y * dir.z + localHemiSphereDir.z * Nt.z,
        theta1); 	
}




inline bool DisneyBRDF(const DisneyBRDFParam & param, const Ray3f & ray, const glm::dvec3 & wi, const SurfaceData & surface, /*HitInfo & hitInfo,*/ Color & attenuation /*, Ray3f & scattered*/)
{
	float cosTheta = glm::dot(surface.normal, -ray.direction);
	float reflective = Fresnel(1.0f, 1.4f, cosTheta);

	//bool bIsSpecular = SysRandom::Random() < (param.metallic /*+ reflective*/);
	// glm::dvec3 reflected = glm::reflect(ray.direction, surface.normal);

	// const float rayRoughness = bIsSpecular ? param.roughness : 1.0f;
	// const glm::vec3 reflectDirection = bIsSpecular ? reflected : surface.normal;
	// const glm::vec3 atten = bIsSpecular ? glm::lerp(glm::vec3(1.0), param.color, param.metallic) * param.specularScale : param.color;


	/* BRDF

	*/ 

	glm::vec3 reflectance = glm::vec3(0);

	auto wm = glm::normalize(-ray.direction + wi);

	float dotNL = glm::dot(surface.normal, -ray.direction);
	float dotNV = glm::dot(surface.normal, wi);
    float diffuseWeight = (1.0f - param.metallic);
 	float diffuse = EvalDisneyDiffuse(param, dotNL, dotNV);

	auto specularFactor = reflective/*EvalDisneySpecular(param, wm, wi, -ray.direction)*/;
	auto specular = glm::lerp(param.specularTint, param.color, param.metallic) * specularFactor;


	reflectance += diffuse * param.color * diffuseWeight;
	reflectance += specular;


	/*
	glm::dvec2 polarSet = random_polar(rayRoughness);
	glm::dvec3 result = glm::dvec3(make_GGXRandom(reflectDirection, polarSet));
	const float EPSILON = 0.0000001f;
	if(glm::dot(result, result) < EPSILON * EPSILON) result = reflectDirection;
    glm::dvec3 outDirection = glm::normalize(result);
    outDirection = outDirection.x != outDirection.x ? reflectDirection : outDirection;


    scattered = {surface.position, outDirection};
    hitInfo.emission = param.emission;
	hitInfo.wi = bIsSpecular ? outDirection : surface.normal;
	hitInfo.nextEvent = bIsSpecular ? HITEVENT::Specular : HITEVENT::Diffuse;
	*/

	attenuation.value = reflectance;

	return true;
}

PBMaterial::PBMaterial() : shader(DefaultBRDFShader)
{

}

glm::vec3 PBMaterial::Albedo(const SurfaceData & surface) const 
{
    DisneyBRDFParam param;
    {
        param.ior = ior;
        param.metallic = metallic;
        param.roughness = roughness;
        param.color = color;
        param.emission = emission;
        param.normal = surface.normal;
        param.specularScale = specularScale;
    };
    if (shader) param = shader(surface, *this);

	return param.color;
}


bool PBMaterial::scatter(const Ray3f & ray, const glm::dvec3 & wi, const SurfaceData & surface, /*HitInfo & hitInfo,*/ Color & attenuation/*, Ray3f & scattered*/) const
{
    DisneyBRDFParam param;
    {
        param.ior = ior;
        param.metallic = metallic;
        param.roughness = roughness;
        param.color = color;
        param.emission = emission;
        param.normal = surface.normal;
        param.specularScale = specularScale;
    };
    if (shader) param = shader(surface, *this);
    Color c;
    //attenuation.value = color;
	return DisneyBRDF(param, ray, wi, surface, /*hitInfo,*/ attenuation/*, scattered*/);
}

DisneyBRDFParam PBMaterial::MakeParam(const SurfaceData& surface) const
{
    DisneyBRDFParam param;
    {
        param.ior = ior;
        param.metallic = metallic;
        param.roughness = roughness;
        param.color = color;
        param.emission = emission;
        param.normal = surface.normal;
        param.specularScale = specularScale;
		param.specularTint = glm::lerp(glm::vec3(1.0f), param.color, metallic);
    };

    if (shader) param = shader(surface, *this);

    return param;
}

bool PBMaterial::sampleBsdf(const SurfaceData & surface, const Ray3f & ray, BsdfSample & bsdfSample) const
{
	DisneyBRDFParam param = MakeParam(surface);
	float cosTheta = glm::dot(surface.normal, -ray.direction);
	float reflective = Fresnel(1.0f, 1.4f, cosTheta);

	bool bIsSpecular = SysRandom::Random() < (param.metallic /*+ reflective*/);
	//bIsSpecular = false;

	glm::dvec3 reflected = glm::reflect(ray.direction, surface.normal);

	const float rayRoughness = bIsSpecular ? param.roughness : 1.0f;
	const glm::vec3 reflectDirection = bIsSpecular ? reflected : surface.normal;
	const glm::vec3 atten = bIsSpecular ? glm::lerp(glm::vec3(1.0), param.color, param.metallic) * param.specularScale : param.color;


	/* BRDF

	*/ 
	glm::dvec2 polarSet = random_polar(rayRoughness);
	glm::dvec3 result = glm::dvec3(make_GGXRandom(reflectDirection, polarSet));
	const float EPSILON = 0.0000001f;
	if(glm::dot(result, result) < EPSILON * EPSILON) result = reflectDirection;
    glm::dvec3 outDirection = glm::normalize(result);
    outDirection = outDirection.x != outDirection.x ? glm::dvec3(reflectDirection) : outDirection;


	SampleDisneyBsdf(surface, param, -ray.direction, outDirection, bsdfSample);
	bsdfSample.reflectance = bIsSpecular ? bsdfSample.reflectance : atten;
	bsdfSample.wi = outDirection; //bIsSpecular ? outDirection : surface.normal;

    // scattered = {surface.position, outDirection};
    // hitInfo.emission = param.emission;
	// hitInfo.wi = bsdfSample.reflectance; //bIsSpecular ? outDirection : surface.normal;
	// hitInfo.nextEvent = bIsSpecular ? HITEVENT::Specular : HITEVENT::Diffuse;
	// attenuation.value = atten;	

	return true;
}

 
