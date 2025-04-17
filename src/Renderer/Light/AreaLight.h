#pragma once
#include "ILight.h"
#include "../Random/SysRandom.h"



class AreaLight : public ILight
{
public:
	glm::vec3 radiance;
	glm::dvec3 position;
	glm::quat rotation;
	float width;
	float height;
	bool visible;

	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal, glm::dvec3 direction, HITEVENT event) const override
	{
		glm::vec3 local_pos = glm::inverse(rotation) * glm::vec3(surfacePosition - position);
		// float power = std::abs(local_pos.x) < width && std::abs(local_pos.y) < height && local_pos.z > 0 ? 1.0f : 0.0f;
		auto delta = position - surfacePosition;

		glm::vec3 lightDirection = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
		float power = glm::clamp(glm::dot(glm::vec3(-direction), lightDirection), 0.0f, 1.0f);
		
		return power * radiance / (float)glm::distance2(surfacePosition, position);
	}
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const override
	{
		glm::vec3 local_pos = glm::vec3(SysRandom::Random() * width, SysRandom::Random() * height, 0.0f);
		glm::dvec3 worldPos = glm::dvec3(rotation * local_pos) + position;

		return glm::normalize(worldPos - surfacePosition);
	}

	virtual glm::dvec3 Position() const override {return position;}
};
