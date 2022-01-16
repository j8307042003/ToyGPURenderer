#include "PBMaterial.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../Random/SysRandom.h"
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

inline glm::dvec2 random_polar(float roughness) {
	float r1 = SysRandom::Random();
	float r2 = SysRandom::Random();

	// float theta1 = atan((roughness * sqrt(r1)) / (sqrt(1 - r1)));
	float theta1 = glm::acos(sqrt((1.0f - r1) / ((roughness * roughness - 1.0f) * r1 + 1.0f)));
	return glm::dvec2(theta1, 2 * 3.1415926f * r2);	
}

inline void make_coordinateSys(const glm::dvec3 & normal, glm::dvec3 & normal_t,  glm::dvec3 & normal_b) {
	if (abs(normal.x) > abs(normal.y))
		normal_t = glm::dvec3(normal.z, 0, -normal.x);
	else
		normal_t = glm::dvec3(0, -normal.z, normal.y);


	normal_b = glm::cross(normal, normal_t);		
}

inline glm::dvec3 uniformSampleHemisphere(float r1, float r2) 
{ 
    float sinTheta = glm::sqrt(1 - r1 * r1); 
    float phi = r2; 
    float x = sinTheta * glm::cos(phi); 
    float z = sinTheta * glm::sin(phi); 
    return glm::dvec3(x, r1, z); 
} 
 


inline glm::dvec4 make_GGXRandom(glm::dvec3 dir, glm::dvec2 polarSet) {
	glm::dvec3 Nb, Nt;
	make_coordinateSys(dir, Nb, Nt);

	float theta1 = glm::cos(polarSet.x);
	glm::dvec3 localHemiSphereDir = uniformSampleHemisphere(theta1, polarSet.y);

    return glm::dvec4( 
        localHemiSphereDir.x * Nb.x + localHemiSphereDir.y * dir.x + localHemiSphereDir.z * Nt.x, 
        localHemiSphereDir.x * Nb.y + localHemiSphereDir.y * dir.y + localHemiSphereDir.z * Nt.y, 
        localHemiSphereDir.x * Nb.z + localHemiSphereDir.y * dir.z + localHemiSphereDir.z * Nt.z,
        theta1); 	
}

bool PBMaterial::scatter(const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered) const
{
	attenuation.value = color;

	glm::dvec3 reflected = glm::reflect(ray.direction, surface.normal);

	glm::dvec2 polarSet = random_polar(1.0f);
	glm::dvec4 result = make_GGXRandom(surface.normal, polarSet);
    glm::dvec3 outDirection = result;
    scattered = {surface.position, outDirection};
    hitInfo.emission = emission;
	return true;
}
 
