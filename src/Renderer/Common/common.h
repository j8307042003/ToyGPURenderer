#pragma once
#include <algorithm>

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
