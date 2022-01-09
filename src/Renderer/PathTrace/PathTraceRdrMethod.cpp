#include "PathTraceRdrMethod.h"
#include <limits>


glm::vec3 PathTraceRdrMethod::Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes)
{
	auto ray = SampleCamRay(rdrData.camData, rdrData.camPosition, rdrData.camDirection, filmRes, glm::vec2(x, y));

	const ShapesData& shapesData = rdrData.sceneData->shapesData;

	float hitDist = std::numeric_limits<float>::max();
	bool bHitAny = false;
	glm::vec3 rayHitPosition;
	glm::vec3 rayHitDirection;

	for (int i = 0; i < rdrData.sceneData->shapes.size(); ++i)
	{
		auto& shape = rdrData.sceneData->shapes[i];

		glm::vec3 hit = {};
		glm::vec3 direction = {};
		bool bHit = false;

		switch (shape.type)
		{
			case ShapeType::Triangle:
				break;
			case ShapeType::Sphere:
				const auto sphereData = shapesData.spheres[shape.primitiveId];
				const auto pos = shapesData.positions[sphereData.x];
				const float radius = shapesData.radius[sphereData.y];

				glm::vec3 hit;
				glm::vec3 direction;
				bHit = IntersectSphere(pos, radius, ray, hit, direction);
				break;
		}

		float d = glm::length(ray.origin - hit);

		if (bHit && d < hitDist)
		{
			hitDist = d;
			rayHitPosition = hit;
			rayHitDirection = direction;
			bHitAny = true;
		}
	}

	return bHitAny ? glm::vec3(1, 1, 1) : glm::vec3(0, 0, 0);

}
