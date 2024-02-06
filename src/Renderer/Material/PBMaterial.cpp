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
	specular = DisneyFresnel(surface, param, -ray.direction, wm, wi);


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
        param.roughness = std::max(0.01f, std::min(0.99f, roughness));
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
        param.roughness = std::max(0.01f, std::min(0.99f, roughness));
        param.color = color;
        param.emission = emission;
        param.normal = surface.normal;
        param.specularScale = specularScale;
    };
    if (shader) param = shader(surface, *this);
    Color c;
    //attenuation.value = color;

	auto wo = glm::normalize(surface.worldToTangent * ray.direction);
	auto tan_wi = glm::normalize(surface.worldToTangent * wi);

	if (wo.y <= 0.0f)
	{
		attenuation.value = {0.0f, 0.0f, 0.0f};
		return true;
	}

	BsdfSample bsdfSample;
	SampleDisneyBsdf(surface, param, wo, tan_wi, bsdfSample);
	attenuation.value = bsdfSample.reflectance;
	return true;
	//return DisneyBRDF(param, ray, wi, surface, /*hitInfo,*/ attenuation/*, scattered*/);
}

DisneyBRDFParam PBMaterial::MakeParam(const SurfaceData& surface) const
{
    DisneyBRDFParam param;
    {
        param.ior = ior;
        param.metallic = metallic;
        param.roughness = std::max(0.01f, std::min(0.99f, roughness));
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

	auto wo = glm::normalize(surface.worldToTangent * -ray.direction);

	float pDiffuse, pSpecular;
 	CalDisneyLobePdfs(param, pDiffuse, pSpecular);

 	float p = SysRandom::Random();

 	glm::dvec3 wi;
 	if (p <= pDiffuse)
 	{
		glm::dvec2 polarSet = random_polar(1.0f);
		glm::dvec3 result = glm::dvec3(make_GGXRandom({0.0f, 1.0f, 0.0f}, polarSet));
		const float EPSILON = 0.0000001f;
		if(glm::dot(result, result) < EPSILON * EPSILON) result = {0.0f, 1.0f, 0.0f};
		wi = glm::normalize(result);
		wi = wi.x != wi.x ? glm::dvec3(surface.normal) : wi;

	}
	else if (p <= pDiffuse + pSpecular)
	{
		float ax, ay;
		CalculateAnisotropicParams(param.roughness, 0.0f, ax, ay);

		float r1 = SysRandom::Random();
		float r2 = SysRandom::Random();
		glm::vec3 wm = SampleGgxVndfAnisotropic(wo, param.roughness, param.roughness, r1, r2);
		wi = glm::normalize(glm::reflect(-wo, wm));
		wi.y = std::abs(wi.y);
		//glm::dvec2 polarSet = random_polar(std::max(0.05f, param.roughness));
		//glm::dvec3 reflected = glm::reflect(ray.direction, surface.normal);
		//glm::dvec3 result = glm::dvec3(make_GGXRandom(reflected, polarSet));
	}

	SampleDisneyBsdf(surface, param, wo, wi, bsdfSample);

	glm::mat3 tangentToWorld = glm::mat3(surface.tangent, surface.normal, surface.bitangent);
	wi = glm::normalize(tangentToWorld * wi);

	if (wi.x != wi.x)
	{
		std::cout << "wi error " << std::endl;
		wi = surface.normal;
	}

	bsdfSample.reflectance = bsdfSample.reflectance * (float)abs(glm::dot(surface.normal, wi));
	bsdfSample.wi = wi; //bIsSpecular ? outDirection : surface.normal;

    // scattered = {surface.position, outDirection};
    // hitInfo.emission = param.emission;
	// hitInfo.wi = bsdfSample.reflectance; //bIsSpecular ? outDirection : surface.normal;
	// hitInfo.nextEvent = bIsSpecular ? HITEVENT::Specular : HITEVENT::Diffuse;
	// attenuation.value = atten;	

	return true;
}

 
bool PBMaterial::hit(const SurfaceData & surface) const 
{
	return albedo_texture == nullptr ? true : (SampleTex(*albedo_texture, surface.uv).w > 0.01f);
}
