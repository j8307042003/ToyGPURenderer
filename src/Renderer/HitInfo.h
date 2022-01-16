#pragma once
#pragma <glm/vec3.hpp>

enum class HITEVENT
{
	Diffuse,
	Specular,
	Transmition,
	SubsurfaceScattering
};


struct HitInfo
{
	HITEVENT nextEvent;
	int materialIdx;
	glm::vec3 emission;
};