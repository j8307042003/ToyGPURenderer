#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "Renderer/Material/Material.h"
#include "BVH/BVH.h"
#include "Light/PointLight.h"
#include "Light/ILight.h"
#include <vector>
#include <map>
#include <string>
#include "Random/SysRandom.h"

class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};
	bvh_tree tree;

	std::map<std::string, int> materialMap;
	std::map<Shape*, int> shapeMaterialMap;

	std::vector<Material*> Materials = {};
	std::vector<ILight*> lights = {};


	void AddShape(Shape * s);
	void AddShape(Shape * s, std::string mat_name);
	void AddModel(std::string modelFile, std::string mat_name, float scale = 1);
	void AddMaterial(material * m);
	void AddMaterial(std::string name, material * m);

	void AddMaterial(std::string name, Material * m);

	void AddPointLight(glm::dvec3 position, glm::vec3 radiance, float radius = 0);

	int GetShapeMaterialIdx(Shape * s) const;

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;
};


struct SceneData
{
	ShapesData shapesData;	
	std::vector<ShapeData> shapes;

	//Light
	std::vector<ILight*> lights;

	//Material
	std::vector<Material*> materials;
};

inline Material* GetMaterial(const SceneData & sceneData, int matIdx)
{
	return sceneData.materials[matIdx];
}

inline int GetShapeMatIdx(const SceneData & sceneData, int shapeIdx)
{
	return sceneData.shapes[shapeIdx].matIdx;
}

inline Material* GetShapeMaterial(const SceneData & sceneData, int shapeIdx)
{
	return GetMaterial(sceneData, GetShapeMatIdx(sceneData, shapeIdx));
}

inline ILight* SampleLight(const SceneData & sceneData)
{
    int lightIdx = std::min((int)sceneData.lights.size() - 1, (int)(SysRandom::Random() * sceneData.lights.size()));
	return lightIdx < 0 ? nullptr : sceneData.lights[lightIdx];
}

void MakeSceneData(const Scene & scene, SceneData & sceneData);
#endif