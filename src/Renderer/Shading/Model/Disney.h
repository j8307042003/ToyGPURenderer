#pragma once


static void CalculateAnisotropicParams(float roughness, float anisotropic, float& ax, float& ay)
{
    float aspect = Sqrtf(1.0f - 0.9f * anisotropic);
    ax = Max(0.001f, Square(roughness) / aspect);
    ay = Max(0.001f, Square(roughness) * aspect);
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

    float sinThetaI = sqrt(max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = ni / nt * sinThetaI;

    // Check for total internal reflection
    if(sinThetaT >= 1) {
        return 1;
    }

    float cosThetaT = sqrt(max(0.0f, 1.0f - sinThetaT * sinThetaT));

    float rParallel     = ((nt * cosThetaI) - (ni * cosThetaT)) / ((nt * cosThetaI) + (ni * cosThetaT));
    float rPerpendicuar = ((ni * cosThetaI) - (nt * cosThetaT)) / ((ni * cosThetaI) + (nt * cosThetaT));
    return (rParallel * rParallel + rPerpendicuar * rPerpendicuar) / 2;
}


static glm::vec3 DisneyFresnel(const SurfaceParameters& surface, const glm::vec3& wo, const glm::vec3& wm, const glm::vec3& wi)
{
	float dotHV = abs(glm::dot(wm, wo));

	glm::vec3 tint = CalculateTint(surface.albedo);
	glm::vec3 r0 = SchlickR0FromRelativeIOR(surface.ior) * glm::lerp(glm::vec3(1.0), tint, specularTint);
	r0 = glm::lerp(r0, surface.albedo, surface.metallic);

	float dielectricFresnel = FresnelDielectric(dotHV, 1.0f, surface.ior);
	glm::vec3 metallicFresnel = FresnelSchlick(r0, glm::dot(wi, wm));

	return glm::lerp(glm::vec3(dielectricFresnel), metallicFresnel, surface.metallic);
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
	float fl = FresnelSchlickWeight(dotNL);
	float fv = FresnelSchlickWeight(dotNV);

	float lambert = 1.0f;
	float retro = EvalDisneyRetroDiffuseTerm(dotNL, dotNV, param.roughness);

	float lamert = (1.0 - 0.5 * fl) * (1.0 - 0.5 * fv);

	float invPi = 1.0 / glm::pi<float>();

	return invPi * (lamert + retro);
}


//aspect=sqroot(1−0.9∗anisotropic)
//αx=roughness2/aspect
//αy=roughness2∗aspect
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