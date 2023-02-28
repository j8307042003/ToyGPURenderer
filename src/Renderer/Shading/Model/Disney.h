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

static glm::vec3 DisneyDiffuse(const DisneyBRDFParam & param, float dotNL, float dotNV, float dotLH)
{
	float Rr = 2.0 * param.roughness * dotLH * dotLH;
	float f90 = 0.5 + Rr;
	float dotNLReverse = (1.0 - dotNL);
	float dotNLReverse2 = dotNLReverse * dotNLReverse;
	float dotNVReverse = (1.0 - dotNV);
	float dotNVReverse2 = dotNVReverse * dotNVReverse;

	return (1.0f + (f90 - 1.0f) * dotNLReverse2 * dotNLReverse2 * dotNLReverse) * (1.0f + (f90 - 1.0f) * dotNVReverse2 * dotNVReverse2 * dotNVReverse) * param.color;// / 3.1415926f;

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

	glm::vec3 tint = CalculateTint(glm::vec3(1.0f));

	// -- See section 3.1 and 3.2 of the 2015 PBR presentation + the Disney BRDF explorer (which does their 2012 remapping
	// -- rather than the SchlickR0FromRelativeIOR seen here but they mentioned the switch in 3.2).
	glm::vec3 R0 = Fresnel::SchlickR0FromRelativeIOR(param.ior) * glm::lerp(glm::vec3(1.0f), tint, param.specularTint);
			 R0 = glm::lerp(R0, param.color, param.metallic);

	float dielectricFresnel = Fresnel::Dielectric(dotHV, 1.0f, param.ior);
	glm::vec3 metallicFresnel = Fresnel::Schlick(R0, glm::dot(wi, wm));

	return glm::lerp(glm::vec3(dielectricFresnel), metallicFresnel, param.metallic);
}

static float DisneySpecularD(const DisneyBRDFParam& param, float dotNH)
{
	auto a2 = param.roughness * param.roughness;
	auto dotNH2 = dotNH * dotNH;
	auto d = a2 * dotNH2 + (1.0f - dotNH2);
	return std::min(65504.0f, 1.0f / d * d);
}

static float D_GGX(float a, float dotNH) {
	//float a2 = a * a;
	//float dotNH2 = dotNH * dotNH;
	//float d = dotNH2 * (a2 - 1.0) + 1.0;
	//return std::min(65504.0f, a2 / (3.1415926f * d * d));

	float a2 = dotNH * a;
	if (a2 == 0.0f) return 1.0f;
	float k = a2 / (1.0 - dotNH * dotNH + 2 * 2);
	return k * k * (1.0 / 3.1415926f);
}

static float microfacet_distribution(
	float roughness, const glm::vec3& normal, const glm::vec3& halfway) {
	// https://google.github.io/filament/Filament.html#materialsystem/specularbrdf
	// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
	auto cosine = glm::dot(normal, halfway);
	if (roughness <= 0.0f) roughness = 0.001f;
	if (cosine <= 0 || roughness <= 0.0f) return 0;
	auto roughness2 = roughness * roughness;
	auto cosine2    = cosine * cosine;
	
	return roughness2 / (3.1415926f * (cosine2 * roughness2 + 1 - cosine2) *
														(cosine2 * roughness2 + 1 - cosine2));
}

static float GTR2Aniso(float NDotH, float HDotX, float HDotY, float ax, float ay)
{
	float a = HDotX / ax;
	float b = HDotY / ay;
	float c = a * a + b * b + NDotH * NDotH;
	return 1.0 / (3.1415926 * ax * ay * c * c);
}

static float SmithGAniso(float NDotV, float VDotX, float VDotY, float ax, float ay)
{
	float a = VDotX * ax;
	float b = VDotY * ay;
	float c = NDotV;
	return (2.0 * NDotV) / (NDotV + sqrt(a * a + b * b + c * c));
}



static float GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}

static float GeometrySmith(float NdotV, float NdotL, float Roughness)
{
	float squareRoughness = Roughness * Roughness;
	float k = pow(squareRoughness + 1, 2) / 8;
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);
	return ggx1 * ggx2;
}

static float V_SmithGGXCorrelatedFast(float NoV, float NoL, float roughness) {
	float a = roughness;
	float GGXV = NoL * (NoV * (1.0 - a) + a);
	float GGXL = NoV * (NoL * (1.0 - a) + a);
	return 0.5 / (GGXV + GGXL);
}


static void CalDisneyLobePdfs(const DisneyBRDFParam & param, float & pDiffuse, float & pSpecular)
{
	float metallicBRDF   = param.metallic;
	float dielectricBRDF = (1.0f - param.metallic);

	float specularWeight     = (metallicBRDF + dielectricBRDF);
	float diffuseWeight      = dielectricBRDF;

	float norm = 1.0f / (specularWeight + diffuseWeight);

	pSpecular  = specularWeight     * norm;
	pDiffuse   = diffuseWeight      * norm;
}

//=========================================================================================================================
static glm::vec3 SampleGgxVndfAnisotropic(const glm::vec3& wo, float ax, float ay, float u1, float u2)
{
		// -- Stretch the view vector so we are sampling as though roughness==1
		glm::vec3 v = glm::normalize(glm::vec3(wo.x * ax, wo.y, wo.z * ay));

		// -- Build an orthonormal basis with v, t1, and t2
		glm::vec3 t1 = (v.y < 0.9999f) ? glm::normalize(glm::cross(v, {0.0f, 1.0f, 0.0f})) : glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 t2 = glm::cross(t1, v);

		const float pi = 3.1415926f;

		// -- Choose a point on a disk with each half of the disk weighted proportionally to its projection onto direction v
		float a = 1.0f / (1.0f + v.y);
		float r = sqrt(u1);
		float phi = (u2 < a) ? (u2 / a) * pi : pi + (u2 - a) / (1.0f - a) * pi;
		float p1 = r * glm::cos(phi);
		float p2 = r * glm::sin(phi) * ((u2 < a) ? 1.0f : v.y);

		// -- Calculate the normal in this stretched tangent space
		glm::vec3 n = p1 * t1 + p2 * t2 + sqrt(std::max(0.0f, 1.0f - p1 * p1 - p2 * p2)) * v;

		// -- unstretch and normalize the normal
		return glm::normalize(glm::vec3(ax * n.x, n.y, ay * n.z));
}



static void SampleDisneyBsdf(const SurfaceData & surface, const DisneyBRDFParam & param, const glm::vec3 & wo, const glm::vec3 & wi, BsdfSample & bsdfSample)
{
	auto wm = glm::normalize(wo + wi);
	auto F = DisneyFresnel(surface, param, wo, wm, wi);

	float ax, ay;
	CalculateAnisotropicParams(param.roughness, 0.0f, ax, ay);

	//float G1v = GGX::SeparableSmithGGXG1(wo, wm, ax, ay);
	//glm::vec3 specular = G1v * F;
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float dotNL = std::min(1.0f, std::max(0.0f, glm::dot(up, wi)));
	float dotNV = std::min(1.0f, std::max(0.0f, glm::dot(up, wo)));
	float dotNH = std::min(1.0f, std::max(0.0f, glm::dot(up, wm)));
	float dotLH = std::min(1.0f, std::max(0.0f, glm::dot(glm::vec3(wi), glm::vec3(wm))));
	//auto diffuse = EvalDisneyDiffuse(param, dotNL, dotNV);
	auto diffuse = DisneyDiffuse(param, dotNL, dotNV, dotLH);  

	//auto d = DisneySpecularD(param, dotNH);
	//auto d = D_GGX(param.roughness * param.roughness, dotNH);
	//auto d = microfacet_distribution(param.roughness, up, glm::vec3(wm));
	//auto g = GeometrySmith(dotNV, dotNL, param.roughness);
	auto g = V_SmithGGXCorrelatedFast(dotNV, dotNL, param.roughness);

	auto d = GTR2Aniso(wm.y, wm.x, wm.z, 1.0f, 1.0f);
	float G1 = SmithGAniso(dotNV, wo.x, wo.z, 1.0f, 1.0f);
	float G2 = G1 * SmithGAniso(dotNL, wi.x, wi.z, 1.0f, 1.0f);

	float pdf = G1 * d / (4.0 * dotNV);
	g = G2;

	auto specular = d * F * G2 / (4.0f * dotNL * dotNV);
	
	bsdfSample.reflectance = dotNL * (diffuse * (1.0f - param.metallic) + specular /pdf);// *pdf;// specular;
}
