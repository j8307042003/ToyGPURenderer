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

inline void MakeSceneData(const Scene & scene, SceneData & sceneData)
{
	sceneData.materials = scene.Materials;
	for (int i = 0; i < scene.shapes.size(); ++i)
	{
		auto pShape = scene.shapes[i];
		auto type = pShape->Type();

		switch(type)
		{
			case ShapeType::Triangle:
			{
				const auto triangleShape = (Triangle*)pShape;
				const auto v0 = triangleShape->Vertices[0];
				const auto v1 = triangleShape->Vertices[1];
				const auto v2 = triangleShape->Vertices[2];

				const auto n = triangleShape->normal;


				int triangleIndex = AddShapesDataTriangle(sceneData.shapesData, 
					glm::dvec3(v0.x, v0.y, v0.z), glm::dvec3(v1.x, v1.y, v1.z), glm::dvec3(v2.x, v2.y, v2.z),
					glm::dvec3(n.x, n.y, n.z)
					);
				auto materialId = scene.GetShapeMaterialIdx(pShape); 		
				ShapeData shapeData = { ShapeType::Triangle , triangleIndex, materialId };
				sceneData.shapes.push_back(shapeData);
			}
				break;
			case ShapeType::Sphere:
			{
				const auto sphereShape = (Sphere*)pShape;
				const auto p = sphereShape->position;

				int sphereIndex = AddShapesDataSphere(sceneData.shapesData, glm::dvec3(p.x, p.y, p.z), sphereShape->radius);
				auto materialId = scene.GetShapeMaterialIdx(pShape); 		
				ShapeData shapeData = { ShapeType::Sphere , sphereIndex, materialId };
				sceneData.shapes.push_back(shapeData);
			}
				break;
			case ShapeType::Plane:
				break;
			default:
				break;
		}
	}


	// Light
	sceneData.lights = scene.lights;

}
