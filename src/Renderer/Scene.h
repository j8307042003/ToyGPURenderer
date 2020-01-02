#pragma once

#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include<vector>


class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};

	void AddShape(Shape * s);
	void AddMaterial(material * m);


};