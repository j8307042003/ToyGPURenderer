#include "PathTraceRdrMethod.h"
#include <limits>
#include "../RayTrace/RayTrace.h"


glm::vec3 PathTraceRdrMethod::Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes)
{
	auto ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y));

	const ShapesData& shapesData = rdrData.sceneData->shapesData;

	glm::vec3 rayHitPosition;
	glm::vec3 rayHitNormal;
	int shapeIndex = -1;
	bool bHitAny = RayTrace(*rdrData.sceneData, ray, rayHitPosition, rayHitNormal, shapeIndex);


	glm::vec3 color = {};

	if(bHitAny)
	{
        int matId = rdrData.sceneData->shapes[shapeIndex].matIdx;
		auto mat = rdrData.sceneData->materials[matId];
        SurfaceData surfaceData = {};
        surfaceData.normal = rayHitNormal;
        HitInfo hitInfo = {};
        Ray3f outRay = {};
        Color att = {};
        mat->scatter(ray, surfaceData, hitInfo, att, outRay);
        color = att.value;
        color = rayHitNormal * 0.5f + 0.5f;
	}

	return color;

}
