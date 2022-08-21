#pragma once

#include <glm/glm.hpp>
#include <algorithm>
#include "../Fresnel.h"
//#include "../GGX.h"
#include "../../Common/common.h"
#include <glm/ext.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/compatibility.hpp>

struct DisneyBRDFParam
{
	float ior = 1.4f;
	float metallic;
	float specularScale;
	float roughness;
	glm::vec3 color;
	glm::vec3 emission;	
	glm::vec3 specularTint = {1.0f, 1.0f, 1.0f};
	glm::dvec3 position;
	glm::dvec3 normal;
};

inline float Square(float v)
{
	return v * v;
}

static void CalculateAnisotropicParams(float roughness, float anisotropic, float& ax, float& ay)
{
	float aspect = std::sqrtf(1.0f - 0.9f * anisotropic);
	ax = std::max(0.001f, Square(roughness) / aspect);
	ay = std::max(0.001f, Square(roughness) * aspect);
}

static glm::vec3 CalculateTint(const glm::vec3 & baseColor)
{
	// -- The color tint is never mentioned in the SIGGRAPH presentations as far as I recall but it was done in
	// --  the BRDF Explorer so I'll replicate that here.
	float luminance = glm::dot({0.3f, 0.6f, 1.0f}, baseColor);
	return (luminance > 0.0f) ? baseColor * (1.0f / luminance) : glm::vec3(1.0f);
}


static float FresnelDielectric(float cosThetaI, float ni, float nt)
{
	// Copied from PBRT. This function calculates the full Fresnel term for a dielectric material.
	// See Sebastion Legarde's link above for details.

	cosThetaI = clamp(cosThetaI, -1.0f, 1.0f);

	// Swap index of refraction if this is coming from inside the surface
	if(cosThetaI < 0.0f) {
		float temp = ni;
		ni = nt;
		nt = temp;

		cosThetaI = -cosThetaI;
	}

	float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
	float sinThetaT = ni / nt * sinThetaI;

	// Check for total internal reflection
	if(sinThetaT >= 1) {
		return 1;
	}

	float cosThetaT = sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

	float rParallel     = ((nt * cosThetaI) - (ni * cosThetaT)) / ((nt * cosThetaI) + (ni * cosThetaT));
	float rPerpendicuar = ((ni * cosThetaI) - (nt * cosThetaT)) / ((ni * cosThetaI) + (nt * cosThetaT));
	return (rParallel * rParallel + rPerpendicuar * rPerpendicuar) / 2;
}

//F_retro−reflection=RR(FL+FV+FLFV(RR−1))
//FL=(1−cosθl)5
//Fv=(1−cosθv)5
//RR=2∗roughness∗cos2(θd)	
static float EvalDisneyRetroDiffuseTerm(float dotNL, float dotNV, float roughness)
{
	float Rr = 2.0 * roughness * dotNL * dotNL;
	float fl = Fresnel::SchlickWeight(dotNL);
	float fv = Fresnel::SchlickWeight(dotNV);
	return Rr * (fl + fv + fl * fv * (Rr - 1.0));
}

static float EvalDisneyDiffuse(const DisneyBRDFParam & param, float dotNL, float dotNV)
{
	//fd=fLambert(1−0.5FL)(1−0.5FV)+fretro−reflection
	float fl = Fresnel::SchlickWeight(dotNL);
	float fv = Fresnel::SchlickWeight(dotNV);

	float lambert = 1.0f;
	float retro = EvalDisneyRetroDiffuseTerm(dotNL, dotNV, param.roughness);

	float lamert = (1.0 - 0.5 * fl) * (1.0 - 0.5 * fv);

	float invPi = 1.0;// / glm::pi<float>();

	return invPi * (lamert + retro);
}

//aspect=sqroot(1−0.9∗anisotropic)
//αx=roughness2/aspect
//αy=roughness2∗aspect
/*
static glm::vec3 EvalDisneySpecular(const DisneyBRDFParam & param, const glm::vec3 & wm, const glm::vec3 & wi, const glm::vec3 & wo)
{
	fPdf = 0.0f;
	rPdf = 0.0f;

	float dotNL = CosTheta(wi);
	float dotNV = CosTheta(wo);
	if(dotNL <= 0.0f || dotNV <= 0.0f) {
		return glm::vec3(0.0);
	}    

	float ax, ay;
	const float aniso = 0.0f;
	CalculateAnisotropicParams(param.roughness, aniso, ax, ay);

	float d = GGXAnisotropicD(wm, ax, ay);
	float gl = GGXAnisotropicG1(wi, wm, ax, ay);
	float gv = GGXAnisotropicG1(wo, wm, ax, ay);

	float f = DisneyFresnel(surface, wo, wm, wi);
	GgxVndfAnisotropicPdf(wi, wm, wo, ax, ay, fPdf, rPdf);

	return d * gl * gv * f / (4.0f * dotNL * dotNV);
}
*/

static glm::vec3 DisneyFresnel(const SurfaceData & surface, const DisneyBRDFParam& param, const glm::vec3 & wo, const glm::vec3 & wm, const glm::vec3 & wi)
{
	float dotHV = glm::dot(wm, wo);

	glm::vec3 tint = CalculateTint(param.color);

	// -- See section 3.1 and 3.2 of the 2015 PBR presentation + the Disney BRDF explorer (which does their 2012 remapping
	// -- rather than the SchlickR0FromRelativeIOR seen here but they mentioned the switch in 3.2).
	glm::vec3 R0 = Fresnel::SchlickR0FromRelativeIOR(param.ior) * glm::lerp(glm::vec3(1.0f), tint, param.specularTint);
		   R0 = glm::lerp(R0, param.color, param.metallic);

	float dielectricFresnel = Fresnel::Dielectric(dotHV, 1.0f, param.ior);
	glm::vec3 metallicFresnel = Fresnel::Schlick(R0, glm::dot(wi, wm));

	return glm::lerp(glm::vec3(dielectricFresnel), metallicFresnel, param.metallic);
}

static void SampleDisneyBsdf(const SurfaceData & surface, const DisneyBRDFParam & param, const glm::vec3 & wo, const glm::vec3 & wi, BsdfSample & bsdfSample)
{
	auto wm = glm::normalize(wo + wi);
	auto F = DisneyFresnel(surface, param, wo, wm, wi);

	float ax, ay;
	CalculateAnisotropicParams(param.roughness, 0.0f, ax, ay);

    //float G1v = GGX::SeparableSmithGGXG1(wo, wm, ax, ay);
    //glm::vec3 specular = G1v * F;
    float dotNL = std::max(0.0f, glm::dot(glm::vec3(surface.normal), wi));
    float dotNV = std::max(0.0f, glm::dot(glm::vec3(surface.normal), wo));
    auto diffuse = EvalDisneyDiffuse(param, dotNL, dotNV);

	bsdfSample.reflectance = diffuse + F;// specular;
}
