#pragma once
#include "../Scene.h"
#include <glm/vec3.hpp>
#include <limits>

inline bool RayTrace(const SceneData & sceneData, const Ray3f & ray, float t_min, float t_max, glm::dvec3 & rayHitPosition, glm::dvec3 & normal, int & shapeIndex)
{
	float hitDist = std::numeric_limits<float>::max();
	bool bHitAny = false;

    const ShapesData& shapesData = sceneData.shapesData;
    
    float t_min2 = t_min * t_min;
    float t_max2 = t_max * t_max;

	for (int i = 0; i < sceneData.shapes.size(); ++i)
	{
		auto& shape = sceneData.shapes[i];

		glm::dvec3 hit;
		glm::dvec3 direction;
		glm::dvec3 shapeNormal = {};
		bool bHit = false;

		switch (shape.type)
		{
			case ShapeType::Triangle:
            {
				const auto triangleData = shapesData.triangles[shape.primitiveId];
				shapeNormal = shapesData.normals[triangleData.z];
  				shapeNormal = glm::dot(ray.direction, shapeNormal) < 0 ? shapeNormal : -shapeNormal;

				bHit = IntersectTriangle(shapesData.positions[triangleData.x], shapesData.positions[triangleData.y], shapesData.positions[triangleData.z], shapeNormal, ray, hit, direction);

				if (bHit) {
					float d2 = glm::length2(hit - ray.origin);
					if (d2 < t_min2 | d2 > t_max2) bHit = false;
				}
            }
				break;
			case ShapeType::Sphere:
			{
				const auto sphereData = shapesData.spheres[shape.primitiveId];
				const auto pos = shapesData.positions[sphereData.x];
				const float radius = shapesData.radius[sphereData.y];

				bHit = IntersectSphere(pos, radius, ray, t_min, t_max, hit, direction);
				shapeNormal = glm::normalize(hit - pos);
			}
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
