#pragma once

#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "BVH/BVH.h"
#include <vector>
#include <map>
#include <string>

class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};
	bvh_tree tree;

	std::map<std::string, int> materialMap;
	std::map<Shape*, int> shapeMaterialMap;

	void AddShape(Shape * s);
	void AddShape(Shape * s, std::string mat_name);
	void AddModel(std::string modelFile, std::string mat_name, float scale = 1);
	void AddMaterial(material * m);
	void AddMaterial(std::string name, material * m);

	int GetShapeMaterialIdx(Shape * s);

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;
};