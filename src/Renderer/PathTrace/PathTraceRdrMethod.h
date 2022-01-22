#pragma once
#include "IRenderMethod.h"

class BVHTree;
class PathTraceRdrMethod : public IRenderMethod
{
public:
	virtual glm::vec3 Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes) override;

public:
	BVHTree* bvh_tree;
private:
	static const int bvh_depth = 32;
	int bvh_stack[bvh_depth];
};