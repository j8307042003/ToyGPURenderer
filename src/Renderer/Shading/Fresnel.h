#ifndef FRESNEL_H
#define FRESNEL_H

#pragma once
#include <glm/glm.hpp>


class Fresnel
{
public:
	static glm::vec3 Schlick( const glm::vec3 & r0, float radians);
	static float SchlickWeight(float w);
	static float Dielectric(float cosThetaI, float ni, float nt);
	static float SchlickR0FromRelativeIOR(float eta);
};

#endif // !FRESNEL_H
