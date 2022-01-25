#pragma once
#include "ILight.h"
#include "../Random/SysRandom.h"


class PointLight : public ILight
{
public:
	glm::vec3 radiance;
	glm::dvec3 position;
	float radius;

	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal, HITEVENT event) const override
	{
		auto delta = position - surfacePosition;
		auto direction = glm::normalize(delta);
		auto distance = glm::length(delta);
		const float LightSize = 30.0f;
		float att = (LightSize - distance) / LightSize; 

        return (std::max(0.0f, (float)glm::dot(direction, surfaceNormal)) * att) * radiance;
	}
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const override
	{
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
	}

	virtual glm::dvec3 Position() const override {return position;}
};
