#include "PathTraceRdrMethod.h"
#include <limits>
#include "../RayTrace/RayTrace.h"
#include <array>
#include "../Accelerate/BVHStruct.h"
#include <glm/glm.hpp>


glm::vec3 PathTraceRdrMethod::Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes)
{
	/*
	auto ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y));

	const ShapesData& shapesData = rdrData.sceneData->shapesData;

	glm::vec3 rayHitPosition;
	glm::vec3 rayHitNormal;
	int shapeIndex = -1;
	bool bHitAny = RayTrace(*rdrData.sceneData, ray, rayHitPosition, rayHitNormal, shapeIndex);


	glm::vec3 color = {};

	if(bHitAny)
	{
		auto mat = GetShapeMaterial(*rdrData.sceneData, shapeIndex);

        SurfaceData surfaceData = {};
        surfaceData.normal = rayHitNormal;
        HitInfo hitInfo;
        Ray3f outRay;
        Color att;
        mat->scatter(ray, surfaceData, hitInfo, att, outRay);
        color = att.value;
        color = rayHitNormal * 0.5f + 0.5f;
	}
	*/


	const auto cam_ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y));
	Ray3f ray = cam_ray;
	HitInfo hitInfo;
    glm::dvec3 rayHitPosition;
    glm::dvec3 rayHitNormal;
    int shapeIndex = -1;
    
	glm::vec3 radiance = glm::vec3(1.0f, 1.0f, 1.0f);
	bool bAnyLightSample = false;
	const int bounce_depth = 8;


	struct SampleResult
	{
		glm::vec3 radiance;
		glm::vec3 emission;
	};

	std::array<SampleResult, bounce_depth> sampleResults = {};

	int depth = 0;
	for (int i = 0; i < bounce_depth; ++i)
	{
		//bool bHitAny = RayTrace(*rdrData.sceneData, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, shapeIndex);
		bool bHitAny = BHV_Raycast(rdrData.sceneData, *bvh_tree, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, shapeIndex, bvh_depth, bvh_stack);

		depth = i;
        if (i == bounce_depth - 1) {break;};
		if (bHitAny)
		{
			auto mat = GetShapeMaterial(*rdrData.sceneData, shapeIndex);

        	SurfaceData surfaceData = {};
        	surfaceData.normal = rayHitNormal;
        	surfaceData.position = rayHitPosition;
        	Ray3f outRay;
        	Color att;
        	mat->scatter(ray, surfaceData, hitInfo, att, outRay);

        	ray = outRay;
        	//radiance = radiance * att.value + hitInfo.emission;
        	//radiance = outRay.direction * 0.5f + 0.5f;
        	//radiance = rayHitNormal * 0.5f + 0.5f;

        	sampleResults[i] = {att.value, hitInfo.emission};

        	if (glm::length2(hitInfo.emission) > 0.0f)
        	{
        		break;
        	}
		}
		else {
        	//radiance *= 0.0f;
            
            glm::dvec3 unit_direction = ray.direction;
            auto t = 0.5f *(unit_direction.y + 1.0f);
            glm::vec3 r = (1.0f-t)*glm::dvec3(1.0, 1.0, 1.0) + t*glm::dvec3(0.5, 0.7, 1.0);
        	sampleResults[i] = {r};
        	break;
		}


		auto pLight = SampleLight(*rdrData.sceneData);
		if (pLight)
		{
			auto lightDelta = pLight->Position() - ray.origin;
			auto lightRayLength = glm::length(lightDelta);
			Ray3f lightSampleRay = {};
			lightSampleRay.origin = ray.origin;
			lightSampleRay.direction = lightDelta / lightRayLength;
			lightSampleRay.direction = pLight->SampleRay(ray.origin);
	
			glm::dvec3 lightTestPosition;
			glm::dvec3 lightTestNormal;
			//bool bHitObstacle = RayTrace(*rdrData.sceneData, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, shapeIndex);
			bool bHitObstacle = BHV_Raycast(rdrData.sceneData, *bvh_tree, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, shapeIndex, bvh_depth, bvh_stack);
			if (!bHitObstacle)
			{
				sampleResults[i].emission += sampleResults[i].radiance * pLight->Eval(rayHitPosition, hitInfo.wi, hitInfo.nextEvent) * ((float)rdrData.sceneData->lights.size());
			}
		}

	}

    radiance = glm::vec3(1.0f);
	for(int i = depth ; i >= 0; --i)
	{
		radiance = sampleResults[i].radiance * radiance + sampleResults[i].emission;
	}

	
	return radiance;

}
