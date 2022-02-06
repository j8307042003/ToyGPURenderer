#pragma once

#include <glm/common.hpp>
#include "../PBMaterial.h"

/*
materail sys


mat
 input -> shader -> BRDF

*/


DisneyBRDFParam DefaultBRDFShader
	(
		// geomotry data
		const SurfaceData & surface,
		const PBMaterial & material
	)
{

    glm::vec3 color = material.color * (material.albedo_texture != nullptr ? glm::vec3(SampleTex(*material.albedo_texture, surface.uv)) : glm::vec3(1.0f));
	//color = glm::vec3(surface.uv.x, surface.uv.y, 0.0f);
	//color = glm::vec3(surface.uv.x, surface.uv.y, 1 - surface.uv.x - surface.uv.y);
	//color = material.color;
    //color = surface.normal * glm::dvec3(0.5f) + glm::dvec3(0.5f);
	//color = material.albedo_texture != nullptr ? material.color : color;
	//glm::vec3(252.0f / 255.0f, 156.0f / 255.0f, 0.0f);


    DisneyBRDFParam param;
	{
        param.metallic = material.metallic;
        param.roughness = material.roughness;
        param.color = color;
        param.emission = material.emission;
        param.normal = surface.normal;
        param.position = surface.position;
	};
	return param;
}
