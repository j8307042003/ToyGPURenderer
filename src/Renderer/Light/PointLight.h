#pragma once
#include "ILight.h"
#include "../Random/SysRandom.h"


inline glm::vec3 uniformSampleHemisphere__(const float& r1, const float& r2)
{
	// cos(theta) = r1 = y
	// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
	float sinTheta = sqrtf(1 - r1 * r1);
	float phi = 2 * glm::pi<float>() * r2;
	float x = sinTheta * cosf(phi);
	float z = sinTheta * sinf(phi);
	return glm::vec3(x, r1, z);
}



class PointLight : public ILight
{
public:
	glm::vec3 radiance;
	glm::dvec3 position;
	float radius;
	bool visible;

	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal, glm::dvec3 direction, HITEVENT event) const override
	{
		auto delta = position - surfacePosition;
		auto d = glm::normalize(delta);
		auto distance = glm::length(delta);
		const float LightSize = 2000.0f;
		float att = (LightSize - distance) / LightSize; 

		return (std::max(0.0f, (float)glm::dot(direction, surfaceNormal)) * att) * radiance;
		//return (att) * radiance;
	}
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const override
	{
		/*
        auto direction = glm::normalize(position - surfacePosition);
        auto rot = glm::quatLookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
        glm::dvec3 up = rot * glm::dvec3(0.0, 1.0, 0.0);
        glm::dvec3 right = rot * glm::dvec3(1.0, 0.0, 0.0);
        
        double randomDelta = SysRandom::Random() * 2.0 * 3.1415926;
        double randomLength = SysRandom::Random() * radius;
        double rightScale = cos(randomDelta) * randomLength;
        double upScale = sin(randomDelta) * randomLength;
        
        auto rightOffset = right * rightScale;
        auto upOffset = up * upScale;
        auto rayTarget = position + rightOffset + upOffset;
        
		return glm::dvec3(glm::normalize(rayTarget - surfacePosition));
		*/

		auto direction = glm::normalize(position - surfacePosition);
		auto rot = glm::quatLookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
		glm::dvec3 up = rot * glm::dvec3(0.0, 1.0, 0.0);
		glm::dvec3 right = rot * glm::dvec3(1.0, 0.0, 0.0);
		glm::dvec3 forward = rot * glm::dvec3(0.0, 0.0, 1.0);

		glm::dmat3 tangentToWorld = glm::dmat3(right, up, forward);

		const glm::dvec3 localHemispherePos = uniformSampleHemisphere__(SysRandom::Random(), SysRandom::Random());

		glm::dvec3 worldSamplePos = (tangentToWorld * localHemispherePos) + position;
		return glm::normalize(worldSamplePos - surfacePosition);
	}

	virtual glm::dvec3 Position() const override {return position;}
};
