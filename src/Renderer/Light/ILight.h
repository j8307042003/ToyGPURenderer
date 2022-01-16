#pragma once
#pragma <glm/vec3.hpp>

class ILight {
public:
	virtual ~ILight() {}
	virtual glm::vec3 Eval(glm::dvec3 surfacePosition, glm::dvec3 surfaceNormal) const = 0;
	virtual glm::dvec3 Position() const = 0;
	virtual glm::dvec3 SampleRay(glm::dvec3 surfacePosition) const = 0;
};