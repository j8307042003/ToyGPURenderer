#include "PBMaterial.h"


bool PBMaterial::scatter(const Ray3f & ray, const SurfaceData & surface, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered) const
{
	attenuation.value = color;
	return true;
}
 
