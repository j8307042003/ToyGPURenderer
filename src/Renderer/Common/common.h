#ifndef COMMON_H
#define COMMON_H

#pragma once
#include <algorithm>

inline float square(float a) {return a * a;}

inline float clamp(float a, float min, float max)
{
	return std::min(std::max(a, min), max);
}

inline float Fresnel(float etaI, float etaT, float cosThetaI) {
	float sinThetaI = sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
	float sinThetaT = etaI / etaT * sinThetaI;
	if (sinThetaT >= 1) return 1;
	float cosThetaT = sqrt(std::max(0.0, 1.0 - sinThetaT * sinThetaT));

	float Rparl = (etaT * cosThetaI - etaI * cosThetaT) /
				  (etaT * cosThetaI + etaI * cosThetaT);
	float Rparp = (etaI * cosThetaI - etaT * cosThetaT) /
				  (etaI * cosThetaI + etaT * cosThetaT);

	return clamp((Rparl * Rparl + Rparp * Rparp) / 2.0f, 0.0f, 1.0f);
}

inline glm::vec3 FresnelSchlick(const glm::vec3 & r0, float radians)
{
    float exponential = pow(1.0f - radians, 5.0f);
    return r0 + (glm::vec3(1.0) - r0) * exponential;
}


// R = R(0) + (1 - R(0)) (1 - cos theta)^5,
inline float FresnelSchlickWeight(float cosTheta) {
    float m = clamp(1 - cosTheta, 0, 1);
    return (m * m) * (m * m) * m;
}

inline float SchlickR0FromRelativeIOR(float eta)
{
    // https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    return square(eta - 1.0f) / square(eta + 1.0f);
}

inline float Cos2Theta(const glm::vec3& w)
{
    return w.y * w.y;
}

inline float Sin2Theta(const glm::vec3& w)
{
    return glm::max(0.0f, 1.0f - Cos2Theta(w));
}

inline float Tan2Theta(const glm::vec3& w)
{
    return Sin2Theta(w) / Cos2Theta(w);
}


inline float CosTheta(const glm::vec3& w)
{
    return w.y;
}



inline float AbsCosTheta(const glm::vec3& w)
{
    return abs(CosTheta(w));
}



inline float SinTheta(const glm::vec3& w)
{
    return sqrt(Sin2Theta(w));
}



inline float CosPhi(const glm::vec3& w)
{
    float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 1.0f : clamp(w.x / sinTheta, -1.0f, 1.0f);
}

inline float Cos2Phi(const glm::vec3& w)
{
    float cosPhi = CosPhi(w);
    return cosPhi * cosPhi;
}

#endif