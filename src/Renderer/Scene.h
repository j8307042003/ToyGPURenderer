#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "assimp/scene.h"
#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "Renderer/Material/Material.h"
#include "Texture/Texture.h"
#include "BVH/BVH.h"
#include "Light/PointLight.h"
#include "Light/DirectionalLight.h"
#include "Light/ILight.h"
#include <vector>
#include <map>
#include <string>
#include "Random/SysRandom.h"
#include "Shading/BsdfSample.h"

struct Mesh
{
	std::vector<Triangle> triangles;
};

class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};
	bvh_tree tree;

	std::map<std::string, int> materialMap;
	std::map<Shape*, int> shapeMaterialMap;
	std::map<std::string, int> textureMap;

	std::vector<Material*> Materials = {};
	std::vector<ILight*> lights = {};
	std::vector<Texture*> textures = {};

	std::vector<Mesh> meshes = {};


	void AddShape(Shape * s);
	void AddShape(Shape * s, std::string mat_name);
	void AddModel(std::string modelFile, std::string mat_name, Vec3 position = Vec3(), glm::quat rotation = glm::quat(), float scale = 1);
	void AddMaterial(material * m);
	void AddMaterial(std::string name, material * m);

	void AddMaterial(std::string name, Material * m);

	void AddPointLight(glm::dvec3 position, glm::vec3 radiance, float radius = 0);
	void AddDirectionalLight(glm::vec3 direction, glm::vec3 radiance);
	Texture* AddTexture(std::string texId, std::string path);
	Texture* AddTexture(std::string texId, std::string path, TextureWrapping wrapping);
	Texture* AddTexture(std::string texId, const Texture & texture);

	int GetShapeMaterialIdx(Shape * s) const;

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;

private:
	int CreateMaterial(aiMaterial * p_material, const std::string & filePath);
};


struct SceneData
{
	ShapesData shapesData;	
	std::vector<ShapeData> shapes;

	//Light
	std::vector<ILight*> lights;

	//Material
	std::vector<Material*> materials;

	std::vector<Texture *> textures;
};

inline Material* GetMaterial(const SceneData & sceneData, int matIdx)
{
	return sceneData.materials[matIdx];
}

//inline ShapeData* GetShapeData(const SceneData & sceneData, int shapeIdx)
//{
//	return (sceneData.shapes.size() <= shapeIdx) ? nullptr : &sceneData.shapes.data()[shapeIdx];
//}

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


struct SceneIntersectData
{
	glm::dvec3 point;
	glm::dvec3 normal;
	glm::vec2 uv;
	int shapeIdx;
	int materialIdx;
};

class BVHTree;
bool IntersectScene(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, SceneIntersectData & intersect);
bool IntersectScene(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, int* stackBuffer, int stackSize, SceneIntersectData & intersect);

bool EvalMaterialScatter(const Material & mat, const Ray3f & ray, const glm::vec3& wi, const SceneIntersectData & intersect, Color & attenuation);
bool EvalMaterialBRDF(const Material & mat, const Ray3f & ray, const SceneIntersectData & intersect, BsdfSample & bsdfSample);

#endif
