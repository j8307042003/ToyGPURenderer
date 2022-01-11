#pragma once

#include "math/Vec3.h"
#include "shape/Shape.h"
#include "Renderer/Material.h"
#include "Renderer/Material/Material.h"
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

	std::vector<Material*> Materials = {};


	void AddShape(Shape * s);
	void AddShape(Shape * s, std::string mat_name);
	void AddModel(std::string modelFile, std::string mat_name, float scale = 1);
	void AddMaterial(material * m);
	void AddMaterial(std::string name, material * m);

	void AddMaterial(std::string name, Material * m);

	int GetShapeMaterialIdx(Shape * s) const;

	void BuildTree();
	bool RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx)const;
};


struct SceneData
{
	ShapesData shapesData;	
	std::vector<ShapeData> shapes;

	//Light

	//Material
	std::vector<Material*> materials;
};

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
					glm::vec3(v0.x, v0.y, v0.z), glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z),
					glm::vec3(n.x, n.y, n.z)
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

				int sphereIndex = AddShapesDataSphere(sceneData.shapesData, glm::vec3(p.x, p.y, p.z), sphereShape->radius);
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
}
