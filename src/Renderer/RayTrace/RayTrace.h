#pragma once
#include "../Scene.h"
#include <glm/vec3.hpp>
#include <limits>

inline bool RayTrace(const SceneData & sceneData, const Ray3f & ray, glm::vec3 & rayHitPosition, glm::vec3 & normal, int & shapeIndex)
{
	float hitDist = std::numeric_limits<float>::max();
	bool bHitAny = false;

    const ShapesData& shapesData = sceneData.shapesData;
    
	for (int i = 0; i < sceneData.shapes.size(); ++i)
	{
		auto& shape = sceneData.shapes[i];

		glm::vec3 hit = {};
		glm::vec3 direction = {};
		glm::vec3 shapeNormal = {};
		bool bHit = false;

		switch (shape.type)
		{
			case ShapeType::Triangle:
            {
				const auto triangleData = shapesData.triangles[shape.primitiveId];
				shapeNormal = shapesData.normals[triangleData.x];
				bHit = IntersectTriangle(shapesData.positions[triangleData.x], shapesData.positions[triangleData.y], shapesData.positions[triangleData.z], shapeNormal, ray, hit, direction);
				break;
            }
			case ShapeType::Sphere:
				const auto sphereData = shapesData.spheres[shape.primitiveId];
				const auto pos = shapesData.positions[sphereData.x];
				const float radius = shapesData.radius[sphereData.y];

				glm::vec3 hit;
				glm::vec3 direction;
				bHit = IntersectSphere(pos, radius, ray, hit, direction);
				shapeNormal = glm::normalize(hit - pos);
				break;
        }

		float d = glm::length2(ray.origin - hit);

		if (bHit && d < hitDist)
		{
			hitDist = d;
			rayHitPosition = hit;
			shapeIndex = i;
			normal = shapeNormal;
			bHitAny = true;
		}
	}

	return bHitAny;
}
