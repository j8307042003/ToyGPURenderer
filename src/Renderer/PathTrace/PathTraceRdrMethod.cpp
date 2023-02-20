#include "PathTraceRdrMethod.h"
#include <limits>
#include "../RayTrace/RayTrace.h"
#include <array>
#include "../Accelerate/BVHStruct.h"
#include <glm/glm.hpp>
#include "../Texture/Texture.h"
#include <algorithm>
#include <Renderer/Accelerate/BVHStruct.h>

glm::vec3 PathTraceRdrMethod::Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes)
{
	const auto cam_ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y), true);
	Ray3f ray = cam_ray;
	HitInfo hitInfo = {};
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

	const int kTraceDepth = 128;
	int TraceBuffer[kTraceDepth];

	std::array<SampleResult, bounce_depth> sampleResults = {};
	int depth = 0;
	for (int i = 0; i < bounce_depth; ++i)
	{
		SceneIntersectData intersect;
		bool bHitAny = IntersectScene(rdrData.sceneData, ray, 0.001f, 10000.0f, intersect);
		//bool bHitAny = IntersectScene(rdrData.sceneData, *bvh_tree, ray, 0.1f, 10000.0f, intersect);
		

		depth = i;
		if (i == bounce_depth - 1) {break;};
		if (bHitAny)
		{
			auto mat = GetShapeMaterial(*rdrData.sceneData, intersect.shapeIdx);

			BsdfSample bsdfSample;
			EvalMaterialBRDF(*mat, ray, intersect, bsdfSample);


			ray.direction = bsdfSample.wi;
			ray.origin = intersect.point;
			sampleResults[i] = {bsdfSample.reflectance, hitInfo.emission};

			if (glm::length2(hitInfo.emission) > 0.0f)
			{
				break;
			}
		}
		else {
			//radiance *= 0.0f;
			glm::dvec3 unit_direction = ray.direction;
			float t = 0.5f *(unit_direction.y + 1.0f);
			//glm::vec3 r = (1.0f-t)*glm::vec3(1.0, 1.0, 1.0) + t*glm::vec3(0.5, 0.7, 1.0);
			glm::vec3 r = (1.0f-t)*glm::vec3(1.0, 1.0, 1.0) + t*glm::vec3(0.188, 0.513, 1.0);			
			sampleResults[i] = {r * 0.5f, glm::vec3(0.0f)};

			if (rdrData.sceneData->envSources.size() > 0)
			{
				glm::vec3 r = rdrData.sceneData->envSources[0]->Sample(unit_direction, i == 0);
				sampleResults[i] = {r, glm::vec3(0.0f)};
			}

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

			auto sample = pLight->Eval(intersect.point, intersect.normal, hitInfo.nextEvent) * ((float)rdrData.sceneData->lights.size());

			if(glm::dot(sample, glm::vec3(1.0f)) > 0.0f)
			{
				glm::dvec3 lightTestPosition;
				glm::dvec3 lightTestNormal;
				glm::vec2 uv;
				//bool bHitObstacle = RayTrace(*rdrData.sceneData, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, shapeIndex);
				//bool bHitObstacle = BHV_Raycast(rdrData.sceneData, *bvh_tree, lightSampleRay, 0.1f, lightRayLength, lightTestPosition, lightTestNormal, uv, shapeIndex, bvh_depth, bvh_stack);
				bool bHitObstacle = OccuScene(rdrData.sceneData, lightSampleRay, 0.001f, lightRayLength);
				if (!bHitObstacle)
				{

					auto mat = GetShapeMaterial(*rdrData.sceneData, intersect.shapeIdx);
					Color lightScatter;			
					EvalMaterialScatter(*mat, ray, lightSampleRay.direction, intersect, lightScatter);

					sampleResults[i].emission += lightScatter.value * sample;
				}
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
