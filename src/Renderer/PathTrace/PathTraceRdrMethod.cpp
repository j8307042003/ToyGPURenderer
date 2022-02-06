#include "PathTraceRdrMethod.h"
#include <limits>
#include "../RayTrace/RayTrace.h"
#include <array>
#include "../Accelerate/BVHStruct.h"
#include <glm/glm.hpp>
#include "../Texture/Texture.h"
#include <algorithm>

glm::vec3 PathTraceRdrMethod::Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes)
{
	const auto cam_ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y));
	Ray3f ray = cam_ray;
	HitInfo hitInfo;
	int shapeIndex = -1;
	
	glm::vec3 radiance = glm::vec3(1.0f, 1.0f, 1.0f);
	bool bAnyLightSample = false;
	const int bounce_depth = 8;
	glm::vec2 screenCoord = glm::vec2(x, y)/ filmRes;

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
		//bool bHitAny = BHV_Raycast(rdrData.sceneData, *bvh_tree, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, uv, shapeIndex, bvh_depth, bvh_stack);
		SceneIntersectData intersect;
		bool bHitAny = IntersectScene(rdrData.sceneData, *bvh_tree, ray, 0.1f, 10000.0f, intersect);
		

		depth = i;
		if (i == bounce_depth - 1) {break;};
		if (bHitAny)
		{
			auto mat = GetShapeMaterial(*rdrData.sceneData, intersect.shapeIdx);
			Ray3f outRay;
			Color att;
			EvalMaterialScatter(*mat, ray, intersect, hitInfo, att, outRay);

			ray = outRay;
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
			sampleResults[i] = {r * 0.5f};
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
			glm::vec2 uv;
			//bool bHitObstacle = RayTrace(*rdrData.sceneData, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, shapeIndex);
			bool bHitObstacle = BHV_Raycast(rdrData.sceneData, *bvh_tree, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, uv, shapeIndex, bvh_depth, bvh_stack);
			if (!bHitObstacle)
			{
				sampleResults[i].emission += sampleResults[i].radiance * pLight->Eval(intersect.point, hitInfo.wi, hitInfo.nextEvent) * ((float)rdrData.sceneData->lights.size());

				/*
				{
					glm::dvec3 unit_direction = ray.direction;
					auto t = 0.5f *(unit_direction.y + 1.0f);
					glm::vec3 r = (1.0f-t)*glm::dvec3(1.0, 1.0, 1.0) + t*glm::dvec3(0.5, 0.7, 1.0);					
                    sampleResults[i].emission += r * glm::vec3(std::max(0.0, glm::dot(lightSampleRay.direction, hitInfo.wi) * 0.5f) * ((float)rdrData.sceneData->lights.size() + 1));
				}
				*/
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
