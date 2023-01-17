#include "BVHStruct.h"
#include "../Scene.h"
#include <math/Ray.h>
#include <glm/glm.hpp>
#include "AABB.h"
#include <glm/vec2.hpp>
#include <glm/common.hpp>
#include <queue>



glm::ivec2 cal_sah(int start, int end, std::vector<dbvhPrimitiveInfo> & primitives)
{
	int primitiveNum = end - start + 1;
	int mid = (start + end) / 2;
	// leaf node
	dAABB centroid = aabb_default();
	dAABB totalBound = aabb_default();


	for(int i = start; i <= end; ++i) {
		totalBound = merge_aabb(totalBound, primitives[i].bound);
		centroid.min = min(centroid.min, primitives[i].center);
		centroid.max = max(centroid.max, primitives[i].center);
	}

	int dim = aabb_maxExtent(centroid);

	std::nth_element(primitives.begin() + start, primitives.begin() + mid, primitives.begin() + end,
		[dim](const dbvhPrimitiveInfo & a, const dbvhPrimitiveInfo & b){
			return a.center[dim] < b.center[dim];
		}
	);

	dBucketInfo buckets[nBuckets] = {};
	for(int i = start; i <= end; i++) {
		glm::dvec3 posNormalized = aabb_offset(centroid, primitives[i].center);
		int bucketIdx = nBuckets * posNormalized[dim];
		if(bucketIdx >= nBuckets) bucketIdx--;
		buckets[bucketIdx].count++;
		buckets[bucketIdx].bound = merge_aabb(buckets[bucketIdx].bound, primitives[i].bound);
	}

	float costs[nBuckets-1] = {};
	for (int i = 0; i < nBuckets-1; ++i){
		dAABB b0 = aabb_default(), b1 = aabb_default();
		int c0 = 0, c1 = 0;
		for(int j = 0; j <= i; ++j) {
			b0 = merge_aabb(b0, buckets[j].bound);
			c0 += buckets[j].count;
		}

		for(int j = i+1; j < nBuckets; ++j) {
			b1 = merge_aabb(b1, buckets[j].bound);
			c1 += buckets[j].count;
		}

		costs[i] = .125f + (c0 * aabb_surfaceArea(b0) + c1 * aabb_surfaceArea(b1)) / aabb_surfaceArea(totalBound);
	}

	float minCost = costs[0];
	int minCostBucket = 0;
	for (int i = 1 ; i < nBuckets-1 ; ++i) {
		if (costs[i] < minCost) {
			minCost = costs[i];
			minCostBucket = i;
		}
	}
	
	dbvhPrimitiveInfo * primitiveMid = std::partition(&primitives[start], &primitives[end],
		[=] (const dbvhPrimitiveInfo & primitiveInfo) {
			int b = nBuckets * aabb_offset(centroid, primitiveInfo.center)[dim];
			if (b == nBuckets) b--;
			return b <= minCostBucket;
		});

	mid = primitiveMid - &primitives[0];
	if (mid == end) mid--;	
	mid = (start + end) / 2;

	return {mid, dim};
}


/*
struct BVH_node
{
	int idx;
	int isLeaf;
	int primitiveId;
	dAABB boundingBox;

	int left;
	int right;
};
*/

void build_bvh_tree(BVHTree & tree, std::vector<dAABB> boundingBoxs)
{
	auto & nodes = tree.nodes;
	nodes.clear();
	nodes.reserve(boundingBoxs.size() * 2);

	// Primitive ID
	std::vector<dbvhPrimitiveInfo> primitives = std::vector<dbvhPrimitiveInfo>();
	primitives.reserve(boundingBoxs.size());
	for(int i = 0; i < boundingBoxs.size(); ++i) {primitives.push_back({boundingBoxs[i], i});}



	std::queue<glm::ivec3> queue = {};
	queue.push(glm::ivec3(0, 0, boundingBoxs.size() - 1));
	nodes.emplace_back();

	while (!queue.empty())
	{
        auto range = queue.front();
		queue.pop();
		int nodeId = range.x;
		int start = range.y;
		int tail = range.z;

		auto& node = nodes[nodeId];

		node.boundingBox = aabb_default();
		for(int i = start; i <= tail; ++i)
			node.boundingBox = merge_aabb(node.boundingBox, primitives[i].bound);

		if (tail - start >= 4)
		{
			auto split = cal_sah(start, tail, primitives);
			int mid = split.x;

			node.isLeaf = false;
			node.primitiveId = 0;
			node.left = nodes.size();
			node.right = node.left + 1;
			nodes.emplace_back();
			nodes.emplace_back();
			queue.push({node.left, start, mid});
			queue.push({node.right, std::min(mid+1, tail), tail});
		}
		else
		{
			node.idx = 0;
			node.isLeaf = true;
			node.left = start;
			node.right = tail;
			//node.primitiveId = primitives[i].primitiveIdx;
		}
	}


  	tree.nodes.shrink_to_fit();
  	tree.primitives = primitives;
    std::cout << "Build BVH Done!" << std::endl;
}

void bvh_buildTree1(SceneData * sceneData, BVHTree & tree) {
	std::vector<dAABB> boundingBoxList;
	boundingBoxList.reserve(sceneData->shapes.size());

	const ShapesData & shapesData = sceneData->shapesData;
	for (int i = 0; i < sceneData->shapes.size(); ++i) {
		const ShapeData & shapeData = sceneData->shapes[i];
		dAABB aabb;

		int primitiveId = shapeData.primitiveId;
		switch(shapeData.type)
		{
			case ShapeType::Sphere:
			{
				auto sphereData = shapesData.spheres[primitiveId];
				aabb = make_aabb(shapesData.positions[sphereData.x], shapesData.radius[sphereData.y]);
				break;
			}
			case ShapeType::Triangle:
			{
				auto triangleData = shapesData.triangles[primitiveId];
				aabb = make_aabb(shapesData.positions[triangleData.x], shapesData.positions[triangleData.y], shapesData.positions[triangleData.z]);
				break;
			}
		}

		boundingBoxList.push_back(aabb);
	}

	//build_bvh_SAH_d(tree, boundingBoxList);
	build_bvh_tree(tree, boundingBoxList);
}

bool BHV_Raycast(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, glm::dvec3 & hitPos, glm::dvec3 & direction, glm::vec2 & uv, int & primitiveIdx, int MaxDepth, int* bvh_visit_stack) {	
	int stackCount = 1;
	bvh_visit_stack[0] = 0;

	glm::dvec3 pos;
	glm::dvec3 normal;
	glm::vec2 sampleuv;
	bool bEverHit = false;

	float depth = 99999999;

	//glm::dvec3 invdir = safe_invdir(ray.direction);
	glm::dvec3 invdir = 1.0 / ray.direction;
	glm::dvec3 oxinvdir = -ray.origin * invdir;

	while (stackCount != 0 && stackCount <= MaxDepth) {
		stackCount--;
		int treeIdx = bvh_visit_stack[stackCount];
		auto & node = bvhtree.nodes[treeIdx];

		if( !intersect_bbox(ray, t_min, t_max, invdir, node.boundingBox)) continue;

		if (node.isLeaf) {

			//*
			for (int i = node.left; i <= node.right; ++i)
			{
				int id = bvhtree.primitives[i].primitiveIdx;
                //if (id >= sceneData->shapes.size() || id < 0) std::cout << "bad shape Id : " << id << std::endl;
				bool bHit = Ray_PrimitiveIntersect(sceneData, ray, t_min, t_max, id, pos, normal, sampleuv);

				glm::dvec3 origin_to_pos = pos - ray.origin;
				float hitDepth = glm::length2(origin_to_pos);
				if (bHit && hitDepth < depth) {
					depth = hitDepth;
					hitPos = pos;
					primitiveIdx = id;
					direction = normal;
					uv = sampleuv;
					bEverHit = true;
				}				

			}
			//*/
			/*
			int id = node.primitiveId;
			bool bHit = Ray_PrimitiveIntersect(sceneData, ray, t_min, t_max, id, pos, normal, sampleuv);


			glm::dvec3 origin_to_pos = pos - ray.origin;
			float hitDepth = origin_to_pos.x * origin_to_pos.x + origin_to_pos.y * origin_to_pos.y + origin_to_pos.z * origin_to_pos.z;
			if (bHit && hitDepth < depth) {
				depth = hitDepth;
				hitPos = pos;
				primitiveIdx = id;
				direction = normal;
				uv = sampleuv;
				bEverHit = true;
			}
			*/
		}
		else {
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


	return bEverHit;
}

