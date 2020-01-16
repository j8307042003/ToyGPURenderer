#pragma once

#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "BVH/BVH.h"
#include<vector>


class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};
	bvh_tree tree;

	void AddShape(Shape * s);
	void AddMaterial(material * m);

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;
};