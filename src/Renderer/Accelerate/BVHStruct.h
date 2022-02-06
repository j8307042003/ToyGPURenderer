#pragma once
#include "AABB.h"
#include "../Scene.h"
#include "../RayTrace/RayTrace.h"
#include <vector>
class SceneData;

struct BVH_node
{
	int idx;
	int isLeaf;
	int primitiveId;
	dAABB boundingBox;

	int left;
	int right;
};



struct dbvhPrimitiveInfo {
	dbvhPrimitiveInfo(){}
	dbvhPrimitiveInfo(const dAABB& bound, int primitiveIdx) :
		bound(bound),
		primitiveIdx(primitiveIdx),
		center((bound.min + bound.max) / 2.0)
	{

	}
	int primitiveIdx;
	dAABB bound;
	glm::dvec3 center;
};

struct BVHTree
{
    std::vector<BVH_node> nodes;
    std::vector<dbvhPrimitiveInfo> primitives;
};


struct dbvhBuildingNode {
	dAABB bound;
	int primitiveIdx;
	bool isLeaf;

	dbvhBuildingNode* left;
	dbvhBuildingNode* right;
};


struct dBucketInfo {
	int count = 0;
	dAABB bound;
};		

inline void handleBoundingBox(dbvhBuildingNode * node)
{
	if(node == nullptr) return;
	dAABB* right = node->right != nullptr ? &node->right->bound : &node->left->bound;
	node->bound = merge_aabb(node->left->bound, *right);
}

inline void BuildBVH_SAH(int start, int end, std::vector<dbvhPrimitiveInfo> & primitives, dbvhBuildingNode * parent) {
	// std::cout << "Start : " << start << ". end : " << end << std::endl;
	int primitiveNum = end - start + 1;
	int mid = (start + end) / 2;
	// leaf node
	if (primitiveNum <= 2) {
		auto node = new dbvhBuildingNode();
		parent->left = node; 

		node->bound        = primitives[start].bound;
		node->primitiveIdx = primitives[start].primitiveIdx;
		node->isLeaf       = true;
		node->left         = nullptr;
		node->right        = nullptr;
		// std::cout << "Primitive " << node->primitiveIdx << ".  isLeaf " << node->isLeaf << std::endl;
		
		if (primitiveNum > 1) {
			auto node = new dbvhBuildingNode();
			parent->right = node;
			node->bound        = primitives[end].bound;
			node->primitiveIdx = primitives[end].primitiveIdx;
			node->isLeaf       = true;
			node->left         = nullptr; 
			node->right        = nullptr;
		}
	} else {
		dAABB centroid = aabb_default();
		dAABB totalBound = aabb_default();


		for(int i = start; i <= end; ++i) {
			totalBound = merge_aabb(totalBound, primitives[i].bound);
			centroid.min = min(centroid.min, primitives[i].center);
			centroid.max = max(centroid.max, primitives[i].center);
		}

		// std::cout << "Center surface area " << centroid.SurfaceArea() << std::endl;

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
		//std::cout << "mid " << mid << std::endl;
		//std::cout << "min cost " << minCost << ".  primitives " << primitiveNum << std::endl;

		auto left = new dbvhBuildingNode();
		parent->left = left;
		left->primitiveIdx = 0;
		left->isLeaf       = false; 
		left->left         = nullptr; 
		left->right        = nullptr;

		auto right = new dbvhBuildingNode();
		parent->right = right;
		right->primitiveIdx = 0;
		right->isLeaf       = false;
		right->left         = nullptr;
		right->right        = nullptr;

		BuildBVH_SAH(start, mid, primitives, parent->left);
		BuildBVH_SAH(std::min(mid+1, end), end, primitives, parent->right);		

		handleBoundingBox(left);
		handleBoundingBox(right);
		//AABB * rightBound = parent->right != nullptr ? &parent->right->bound : &parent->left->bound; 
		//parent->bound = merge_aabb(parent->left->bound, *rightBound);

	}
}

inline void RecursiveBuild(std::vector<BVH_node> & nodes, dbvhBuildingNode * buildingNode, int & depth) {
	bool bIsLeaf = buildingNode->isLeaf;

	BVH_node node = {};
	node.idx = (int) nodes.size();
	node.primitiveId = buildingNode->primitiveIdx;
	node.boundingBox = buildingNode->bound;
	node.isLeaf = bIsLeaf;
	node.left = -1;
	node.right = -1;
	nodes.push_back(node);

	//std::cout << "Idx " << node.idx << std::endl;
	//std::cout << "box " << node.boundingBox.min.tostring() <<
	//			 ". " << node.boundingBox.max.tostring() << std::endl;


	depth++;
	int d = depth;
	int d2 = depth;
	if (!bIsLeaf) {
		if (buildingNode->left != nullptr) {
			nodes[node.idx].left = (int) nodes.size();
			RecursiveBuild(nodes, buildingNode->left, d);
			if (d > depth) depth = d;
		}
		if (buildingNode->right != nullptr) {
			nodes[node.idx].right = (int) nodes.size();
			RecursiveBuild(nodes, buildingNode->right, d2);
			if (d2 > depth) depth = d2;
		} 
	}

	delete buildingNode;
}





inline void build_bvh_SAH_d(BVHTree & tree, std::vector<dAABB> & boundingBoxs) {
	std::vector<dbvhPrimitiveInfo> primitives = std::vector<dbvhPrimitiveInfo>();
	primitives.reserve(boundingBoxs.size());
	for(int i = 0; i < boundingBoxs.size(); ++i) {
		primitives.push_back({boundingBoxs[i], i});
	}

	tree.nodes.clear();

	dbvhBuildingNode * root = new dbvhBuildingNode();
	BuildBVH_SAH(0, primitives.size()-1, primitives, root);
	root->bound = merge_aabb(root->left->bound, root->right->bound);
	root->isLeaf = false;
	int depth = 1;
	RecursiveBuild(tree.nodes, root, depth);

	//std::cout << "Done output tree max depth " << depth << std::endl;

}

glm::ivec2 cal_sah(int start, int end, std::vector<dbvhPrimitiveInfo> & primitives);

void build_bvh_tree(BVHTree & tree, std::vector<dAABB> boundingBoxs);

inline void bvh_buildTree1(SceneData * sceneData, BVHTree & tree) {
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

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
inline glm::dvec3 safe_invdir(glm::dvec3 d)
{
    float dirx = d.x;
    float diry = d.y;
    float dirz = d.z;
    float ooeps = 1e-5;
    glm::dvec3 invdir;
    invdir.x = 1.0 / (abs(dirx) > ooeps ? dirx : (sgn(dirx) >= 0.f ? ooeps : -ooeps));
    invdir.y = 1.0 / (abs(diry) > ooeps ? diry : (sgn(diry) >= 0.f ? ooeps : -ooeps));
    invdir.z = 1.0 / (abs(dirz) > ooeps ? dirz : (sgn(dirz) >= 0.f ? ooeps : -ooeps));
    return invdir;
}



bool BHV_Raycast(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, glm::dvec3 & hitPos, glm::dvec3 & direction, glm::vec2 & uv, int & primitiveIdx, int MaxDepth, int* bvh_visit_stack);

