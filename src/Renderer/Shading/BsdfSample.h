#pragma once
#include <glm/glm.hpp>

struct BsdfSample
{
	glm::vec3 reflectance;
	glm::vec3 wi;
};