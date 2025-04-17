#pragma once
#include "ILight.h"
#include "../Random/SysRandom.h"
#include <glm/common.hpp>



class SpotLight : public ILight
{
public:
	glm::vec3 radiance;
	glm::dvec3 position;
	glm::quat rotation;
	float degree;
	float falloff;

	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal, glm::dvec3 direction, HITEVENT event) const override
	{

		glm::vec3 local_pos = glm::inverse(rotation) * glm::vec3(surfacePosition - position);

		auto angle = glm::angle(glm::normalize(local_pos), glm::vec3(0.0, 0.0, 1.0));

		float power = 1.0f - glm::smoothstep(falloff, degree, angle);

		return power * radiance / (float)glm::distance2(surfacePosition, position);
	}
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const override
	{
		return glm::normalize(position - surfacePosition);
	}

	virtual glm::dvec3 Position() const override {return position;}
};
