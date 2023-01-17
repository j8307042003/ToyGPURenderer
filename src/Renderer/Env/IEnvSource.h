#pragma once
#pragma <glm/vec3.hpp>



class IEnvSource
{
public:
	virtual glm::vec3 Sample(const glm::dvec3 & direction) = 0;
};