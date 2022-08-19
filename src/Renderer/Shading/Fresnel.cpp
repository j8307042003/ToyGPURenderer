#include "Fresnel.h"
#include <algorithm>
#include <glm/glm.hpp>
#include "../Common/common.h"

inline float Saturate(float v)
{
    if(v < 0.0f) {
        return 0.0f;
    }
    else if(v > 1.0f) {
        return 1.0f;
    }

    return v;	
}


glm::vec3 Fresnel::Schlick(const glm::vec3 & r0, float radians)
{
    float exponential = std::powf(1.0f - radians, 5.0f);
    return r0 + (glm::vec3(1.0f) - r0) * exponential;
}

float Fresnel::SchlickWeight(float w)
{
	float m = Saturate(1.0f - w);
	float m2 = m * m;
	return m * m2 * m2;	
}


float Fresnel::Dielectric(float cosThetaI, float ni, float nt)
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

    float sinThetaI = std::sqrtf(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = ni / nt * sinThetaI;

    // Check for total internal reflection
    if(sinThetaT >= 1) {
        return 1;
    }

    float cosThetaT = std::sqrtf(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

    float rParallel     = ((nt * cosThetaI) - (ni * cosThetaT)) / ((nt * cosThetaI) + (ni * cosThetaT));
    float rPerpendicuar = ((ni * cosThetaI) - (nt * cosThetaT)) / ((ni * cosThetaI) + (nt * cosThetaT));
    return (rParallel * rParallel + rPerpendicuar * rPerpendicuar) / 2;
}

inline float Square(float v)
{
    return v * v;
}

float Fresnel::SchlickR0FromRelativeIOR(float eta)
{
    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    return Square(eta - 1.0f) / Square(eta + 1.0f);
}