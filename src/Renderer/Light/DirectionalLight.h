#pragma once
#include "ILight.h"
#include "../Random/SysRandom.h"
#include <glm/vec3.hpp>

class DirectionalLight : public ILight
{
public:
	glm::vec3 radiance;
	glm::dvec3 direction;

	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal, HITEVENT event) const override
	{
        return (std::max(0.0f, (float)glm::dot(-direction, surfaceNormal))) * radiance;
	}
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const override
	{
		return glm::dvec3(-direction);
	}

	virtual glm::dvec3 Position() const override { return glm::vec3(0); }
};
