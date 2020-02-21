#pragma once

#ifndef BVH_H
#define BVH_H

#include "AABB.h"
#include "shape/Shape.h"
#include <vector>
#include <iostream>

const int bvh_max_child = 2;


struct bvh_node {
	int idx;
	int primitiveId;
	int p0, p1;
	AABB boundingBox;
	uint isLeaf;
	// char p0, p1, p2;
	int left;
	int right;
	int p2;
};



struct bvh_tree {
	std::vector<bvh_node> nodes;
};

inline void build_bvh(bvh_tree & tree, std::vector<AABB> & boundingBoxs) {
	tree.nodes.reserve(boundingBoxs.size() * 2);

}


inline int GetLeftChild(int i) {
	return (i << 1) + 1;
}

inline int GetRightChild(int i) {
	return (i << 1) + 2;
}


inline void build_bvh_simple(bvh_tree & tree, std::vector<AABB> & boundingBoxs) {
	tree.nodes.clear();

	int itemNum = boundingBoxs.size();
	int pow_n = ceil(log2(itemNum))+1;
	int treeMax = pow(2, pow_n);
	int treeMin = pow(2, pow_n-1);


	int treeSize = treeMin + itemNum - 1;
	tree.nodes.reserve(treeSize);



	for(int i = 0; i < treeSize; i++) {
		tree.nodes.push_back({});
	}

	for(int i = treeSize - itemNum; i < treeSize; i++ ){
		bvh_node node;
		node.boundingBox = boundingBoxs[i-(treeSize - itemNum)];
		node.idx = i;
		node.primitiveId = i-(treeSize - itemNum);
		node.isLeaf = true;
		tree.nodes[i] = node;
		std::cout << "Add leaf " << i << std::endl;
	}

	std::cout << "Tree size " << treeSize << std::endl;
	//for (int j = pow_n-1; j >= 0 ; --j ) {
	//	int head = j == 0 ? 0 : pow(2, j-1) - 1;
	//	int tail = head << 1;

		// for (int i = head; i <= tail;i++){
		for (int i = treeSize - itemNum - 1; i >= 0 ;i--){
			std::cout << "Add node " << i << std::endl;
			bvh_node node;
			node.isLeaf = false;
			node.idx = i;
	
			int l = GetLeftChild(i);
			int r = GetRightChild(i);
	
			node.left = l < treeSize ? l : -1;
			node.right = r < treeSize ? r : -1;
			// std::cout << node.left << std::endl;
			// std::cout << node.right << std::endl;
			bvh_node * node_l = nullptr;
			if ( node.left > 0 ) node_l = &tree.nodes[l];
	
			bvh_node * node_r = nullptr;
			if (node.right > 0 ) node_r = &tree.nodes[r];
	
	
			if (node_l != nullptr && node_r != nullptr) {
				node.boundingBox = merge_aabb(node_l->boundingBox, node_r->boundingBox);
			}
			else if (node_l != nullptr){
				node.boundingBox = node_l->boundingBox;
			}
			else if (node_r != nullptr){
				node.boundingBox = node_r->boundingBox;
			}
			else{
				node.boundingBox = aabbZero;
			}
	
			tree.nodes[i] = node;
		}
	// }
}

#endif
