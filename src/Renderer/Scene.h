#ifndef SCENE_H
#define SCENE_H

#pragma once
#include <set>
#include "assimp/scene.h"
#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "Renderer/Material/Material.h"
#include "Texture/Texture.h"
#include "BVH/BVH.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"
#include "Light/AreaLight.h"
#include "Light/DirectionalLight.h"
#include "Light/ILight.h"
#include <vector>
#include <map>
#include <string>
#include "Random/SysRandom.h"
#include "Shading/BsdfSample.h"
#include "Env/IEnvSource.h"
#include "Accelerate/BVHStruct.h"
#include "RayTraceEngine/IRayTraceEngine.h"
#include "Camera/Camera.h"

struct Mesh
{
	std::vector<Triangle> triangles;
};



struct ModelResource
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> uvs;
	std::vector<glm::ivec3> triangles;
    std::vector<unsigned int> materialIdx;
};

class Scene {
public:
	std::vector<Shape*> shapes = {};
	std::vector<material*> materials = {};
	bvh_tree tree;

	std::map<std::string, int> materialMap;
	std::map<Shape*, int> shapeMaterialMap;
	std::map<std::string, int> textureMap;
	std::map<std::string, int> textureFileMap = {};


	std::vector<ModelResource> modelResources = {};
	std::map<std::string, int> modelRecourcesMap = {};

	std::vector<Material*> Materials = {};
	std::vector<ILight*> lights = {};
	std::vector<Texture*> textures = {};
	std::vector<IEnvSource*> envSources = {};
	std::vector<std::string> envSourceNames = {};

	std::map<std::string, std::string> meshFileMap = {};

	std::vector<Mesh> meshes = {};
	std::string cameraModel = "";
	int imageWidth = 720;
	int imageHeight = 720;

	void AddShape(Shape * s);
	void AddShape(Shape * s, std::string mat_name);
	void AddModel(std::string modelFile, std::string mat_name, Vec3 position = Vec3(), glm::quat rotation = glm::quat(), float scale = 1);
    bool LoadMeshResources(const std::string & path, ModelResource * & modelResource);
	void AddEnv(IEnvSource * envSource);
	void AddMaterial(material * m);
	void AddMaterial(std::string name, material * m);

	void AddMaterial(std::string name, Material * m);

	void AddPointLight(glm::dvec3 position, glm::vec3 radiance, float radius = 0, bool visible = false);
	void AddSpotLight(glm::dvec3 position, glm::quat rotation, glm::vec3 radiance, float degree, float falloff);
	void AddAreaLight(glm::dvec3 position, glm::quat rotation, glm::vec3 radiance, float width, float height, bool visible = false);
	void AddDirectionalLight(glm::vec3 direction, glm::vec3 radiance);
	void AddEnvSource(const std::string & path, float scale = 1.0f, float sampleScale = 1.0f);
	void AddEnvResource(const std::string & path); 
	Texture* AddTexture(std::string texId, std::string path);
	Texture* AddTexture(std::string texId, std::string path, TextureWrapping wrapping);
	Texture* AddTexture(std::string texId, const Texture& texture);
	Texture* AddTexture(std::string texId, std::string path, Texture * texture);
	Texture* AddExrTexture(const std::string & texId, const std::string & path);

	int GetShapeMaterialIdx(Shape * s) const;

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;

private:
	int CreateMaterial(aiMaterial * p_material, const std::string & filePath);
	void DumpMaterialTextures(aiMaterial* p_material, const std::string& filePath, std::map<std::string, std::string>& map, std::map<std::string, TextureWrapping>& wrappingMap);
};


struct DynamicMesh
{
    ModelResource* resources;
    glm::vec3 position;
    glm::vec3 eulerRotation;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;

    float scale;
    const char* name;
};

struct SceneData
{
	ShapesData shapesData;	
	std::vector<ShapeData> shapes;

	//Light
	std::vector<ILight*> lights;

	unsigned int envIdx;
	std::vector<IEnvSource*> envSources;

	//Material
	std::vector<Material*> materials;
    
    std::vector<DynamicMesh> dynamicObjs;

	std::vector<Texture *> textures;

	BVHTree *bvh_tree;
    
    CameraData cameraData;

	IRayTraceEngine* pRayTraceEngine;

	std::vector<std::string> envSourceNames;
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

inline void SampleLight(const SceneData & sceneData, const glm::dvec3 & surfacePosition, const glm::dvec3 & surfaceNormal, ILight* &  light, glm::dvec3 & direction, glm::vec3 & lightPower)
{
    int lightIdx = std::min((int)sceneData.lights.size() - 1, (int)(SysRandom::Random() * sceneData.lights.size()));
	light = lightIdx < 0 ? nullptr : sceneData.lights[lightIdx];

	if (light == nullptr) return;


	HITEVENT hitevent = {}; // todo. actually handle this.
	direction = light->SampleRay(surfacePosition);
	lightPower = light->Eval(surfacePosition, surfaceNormal, direction, hitevent);
}

void MakeSceneData(const Scene & scene, SceneData & sceneData, bool enableEmbree);


struct SceneIntersectData
{
	glm::dvec3 point;
	glm::dvec3 normal;
	glm::dvec3 tangent;
	glm::vec2 uv;
	int shapeIdx;
	int materialIdx;
};

bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData & intersect);
bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, int* stackBuffer, int stackSize, SceneIntersectData & intersect);
bool OccuScene(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max);

bool EvalMaterialScatter(const Material & mat, const glm::vec3 & view, const glm::vec3& wi, const SceneIntersectData & intersect, Color & attenuation);
bool EvalMaterialBRDF(const Material & mat, const Ray3f & ray, const SceneIntersectData & intersect, BsdfSample & bsdfSample);
glm::vec3 EvalMaterialEmission(const Material& mat, const SceneIntersectData& intersect);


bool UpdateDynamicsObj(SceneData * sceneData, DynamicMesh * obj);
bool AddDynamicsObj(SceneData * sceneData, ModelResource* resource, const char* name);

#endif
