#include "EmissionMaterial.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "../Random/SysRandom.h"
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include "../Common/common.h"
#include <glm/gtx/compatibility.hpp>
 


EmissionMaterial::EmissionMaterial()
{

}

glm::vec3 EmissionMaterial::Albedo(const SurfaceData & surface) const 
{
    return emission;
}


bool EmissionMaterial::scatter(const glm::dvec3& view, const glm::dvec3 & wi, const SurfaceData & surface, /*HitInfo & hitInfo,*/ Color & attenuation/*, Ray3f & scattered*/) const
{
	attenuation.value = emission;
	return true;
}

bool EmissionMaterial::sampleBsdf(const SurfaceData & surface, const Ray3f & ray, BsdfSample & bsdfSample) const
{
	auto wo = glm::normalize(surface.worldToTangent * -ray.direction);

 	glm::dvec3 wi;
	wi = glm::normalize(glm::reflect(-wo, glm::vec3(0.0f, 1.0f, 0.0f)));
	wi.y = std::abs(wi.y);

	bsdfSample.reflectance = emission;
	bsdfSample.wi = wi;

	return true;
}

 
bool EmissionMaterial::hit(const SurfaceData & surface) const 
{
	return true;
}
