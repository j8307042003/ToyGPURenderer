#include "Scene.h"
#include <typeinfo>
#include <map>
#include <string>


#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/gtx/transform.hpp"

void Scene::AddShape(Shape * s) {
	shapes.push_back(s);
}

void Scene::AddShape(Shape * s, std::string mat_name) {
	std::map<std::string,int>::iterator it = materialMap.find(mat_name);
	if (it == materialMap.end()) {
		std::cout << "Add Shape Failed. Due to add Shape with not existed material : " + mat_name << std::endl;
		return;
	}

	shapes.push_back(s);
	shapeMaterialMap[s] = it->second;
}


void Scene::AddModel(std::string modelFile, std::string mat_name) {
	std::map<std::string,int>::iterator it = materialMap.find(mat_name);
	if (it == materialMap.end()) {
		std::cout << "Add Shape Failed. Due to add Shape with not existed material : " + mat_name << std::endl;
		return;
	}

    aiPropertyStore* props = aiCreatePropertyStore();

    const float scale = 0.03;
	// auto scene = aiImportFile("dragon.obj",aiProcessPreset_TargetRealtime_MaxQuality);
	auto scene = aiImportFileExWithProperties(modelFile.c_str(), aiProcess_Triangulate, NULL, props);
	if (scene) {
		std::cout << "num of mNumMeshes : " << scene->mNumMeshes << std::endl;
		for( int i = 0; i < scene->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[i];

			if (mesh->mNumVertices != 3)
				std::cout << "there's a mesh not triangle : " << mesh->mNumVertices << std::endl;

			for (int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];
				// std::cout << "Num of indices : " << face.mNumIndices << std::endl; 
				Vec3 vectors[3];
				glm::mat4 model = glm::mat4();
				model = glm::scale(model, glm::vec3(scale, scale, scale));				
				for (int k = 0; k < face.mNumIndices; k++) {
					aiVector3D & v = mesh->mVertices[face.mIndices[k]];
					auto m = (model * glm::vec4(v.x, v.y, v.z, 1.0));
					// vectors[k] = {m.x, m.y, m.z};
					vectors[k] = {v.x * scale, v.y * scale, v.z * scale - 30};
				}

				Triangle * triangle = new Triangle(vectors[0], vectors[1], vectors[2]);
				shapes.push_back(triangle);
				shapeMaterialMap[triangle] = it->second;
				if (shapes.size() > 100000) return;
			}		


			std::cout << "there's a mesh not triangle : " << mesh->mNumVertices << std::endl;

		}
	}

}



void Scene::AddMaterial(material * s) {
	materials.push_back(s);
}

void Scene::AddMaterial(std::string name, material * m) {
	materials.push_back(m);
	materialMap[name] = materials.size()-1;
}

int Scene::GetShapeMaterialIdx(Shape * s) {
	std::map<Shape*,int>::iterator it = shapeMaterialMap.find(s);
	if (it == shapeMaterialMap.end()) {
		return -1;
	}

	return it->second;
}



void Scene::BuildTree() {
	std::vector<AABB> boundingBoxList;
	boundingBoxList.reserve(shapes.size());
	for (int i = 0; i < shapes.size(); ++i) {
		Shape & s = *shapes[i];
		AABB aabb;
		const std::type_info & type = typeid(s);

		if( type == typeid(Sphere))
		  	aabb = make_aabb(*dynamic_cast<Sphere*>(&s));
		else if( type == typeid(Triangle))
		  	aabb = make_aabb(*dynamic_cast<Triangle*>(&s));
		else if( type == typeid(Plane))
		  	aabb = make_aabb(*dynamic_cast<Plane*>(&s));


		boundingBoxList.push_back(aabb);
	}

	build_bvh_simple(tree, boundingBoxList);
}


bool Scene::RayCastTest(const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx) const {
	std::vector<int> visitList;
	visitList.reserve(tree.nodes.size());
	visitList.push_back(0);

	Vec3 pos;
	Vec3 dir;
	bool bEverHit = false;

	float depth = 99999999;
	int runId = 0;

	// std::cout << "Ray Test " << ray.origin.tostring() << " " << ray.dir.tostring() << std::endl;
	while(runId < visitList.size()) 
	{
		bvh_node node = tree.nodes[visitList[runId]];
		bool bIntersect = node.boundingBox.RayIntersect(ray);
		// std::cout << "id " << bIntersect << "  " << node.idx << " " << node.boundingBox.min.tostring() << "  " << node.boundingBox.max.tostring() << std::endl  ;
		if (bIntersect) {
			if (node.isLeaf) {
				int id = node.primitiveId;
				bool bHit = shapes[id]->RayCastTest(&ray, pos, dir);
				float hitDepth = (pos - ray.origin).length();
	
				if (bHit && hitDepth < depth ){
					depth = hitDepth;
					hitPos = pos;
					idx = id;
					direction = dir;
					bEverHit = true;
				}
			}
			else {
				if (node.left > 0 ) visitList.push_back(node.left);
				if (node.right > 0 ) visitList.push_back(node.right);
			}
		}

		runId++;
	}

	return bEverHit;
}


//bool RayCast(const std::vector<Shape*> * shapes, const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx) {
//	float depth = 9999999;
//	bool bEverHit = false;
//	Vec3 pos;
//	Vec3 dir;
//	for(int k = 0 ; k < shapes->size() ; k++){
//		bool bHit = (*shapes)[k]->RayCastTest(&ray, pos, dir);
//		//lazy depth test
//		float hitDepth = (pos - ray.origin).length();
//
//		if (bHit && hitDepth < depth ) {
//			depth = hitDepth;
//			hitPos = pos;
//			idx = k;
//			direction = dir;
//			bEverHit = true;
//		}
//	}
//
//	return bEverHit;
//}
