#include "BVHStruct.h"
#include "../Scene.h"
#include <math/Ray.h>
#include <glm/glm.hpp>
#include "AABB.h"
#include <glm/vec2.hpp>
#include <glm/common.hpp>



bool BHV_Raycast(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, glm::dvec3 & hitPos, glm::dvec3 & direction, int & primitiveIdx, int MaxDepth, int* bvh_visit_stack) {	
	int stackCount = 1;
	bvh_visit_stack[0] = 0;

	glm::dvec3 pos;
	glm::dvec3 normal;
	bool bEverHit = false;

	float depth = 99999999;

	//glm::dvec3 invdir = safe_invdir(ray.direction);
	glm::dvec3 invdir = 1.0 / ray.direction;
	glm::dvec3 oxinvdir = -ray.origin * invdir;

	while (stackCount != 0) {
		stackCount--;
		int treeIdx = bvh_visit_stack[stackCount];
		
		auto & node = bvhtree.nodes[treeIdx];
		if (node.isLeaf > 0) {
			int id = node.primitiveId;
			bool bHit = Ray_PrimitiveIntersect(sceneData, ray, t_min, t_max, id, pos, normal);


			glm::dvec3 origin_to_pos = pos - ray.origin;
			float hitDepth = origin_to_pos.x * origin_to_pos.x + origin_to_pos.y * origin_to_pos.y + origin_to_pos.z * origin_to_pos.z;
			if (bHit && hitDepth < depth) {
				depth = hitDepth;
				hitPos = pos;
				primitiveIdx = id;
				direction = normal;
				bEverHit = true;
			}
		}
		else {
			//glm::vec2 s = fast_intersect_aabb(node.boundingBox, invdir, oxinvdir);
			//bool bIntersect = (s.x <= s.y);
			bool bIntersect = intersect_bbox(ray, t_min, t_max, invdir, node.boundingBox);

			if (bIntersect) {
				if (node.left > 0) {
					bvh_visit_stack[stackCount] = node.left;
					stackCount++;
				}
				if (node.right > 0) {
					bvh_visit_stack[stackCount] = node.right;
					stackCount++;
				}
			}
		}
	}


	return bEverHit;
}

