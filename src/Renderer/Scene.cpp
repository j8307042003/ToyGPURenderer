#include "Scene.h"
#include <typeinfo>
#include <map>
#include <string>


#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/gtx/transform.hpp"
#include "Material/PBMaterial.h"
#include "Accelerate/BVHStruct.h"



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

	Plane * pPlane = dynamic_cast<Plane*>(s);
	if (pPlane != nullptr)
	{
		Triangle * pT1 = &pPlane->t1;
		Triangle * pT2 = &pPlane->t2;
        pT1->uv[0] = Vec3(0.0f, 0.0f, 0.0f);
        pT1->uv[1] = Vec3(0.0f, 1.0f, 0.0f);
        pT1->uv[2] = Vec3(1.0f, 1.0f, 0.0f);
        pT2->uv[0] = Vec3(0.0f, 1.0f, 0.0f);
        pT2->uv[1] = Vec3(1.0f, 0.0f, 0.0f);
        pT2->uv[2] = Vec3(1.0f, 1.0f, 0.0f);
		AddShape(pT1, mat_name);
		AddShape(pT2, mat_name);
	}

	shapeMaterialMap[s] = it->second;
}


void Scene::AddModel(std::string modelFile, std::string mat_name, Vec3 position, float scale) {
	std::map<std::string,int>::iterator it = materialMap.find(mat_name);
	if (it == materialMap.end()) {
		std::cout << "Add Shape Failed. Due to add Shape with not existed material : " + mat_name << std::endl;
		return;
	}

    aiPropertyStore* props = aiCreatePropertyStore();
    std::cout << "Loading Model : " << modelFile << std::endl;

	//auto scene = aiImportFile("dragon.obj",aiProcessPreset_TargetRealtime_MaxQuality);	
	//auto scene = aiImportFileExWithProperties(modelFile.c_str(), aiProcess_Triangulate, NULL, props);
	auto scene = aiImportFileExWithProperties(modelFile.c_str(), aiProcessPreset_TargetRealtime_Fast, NULL, props);
	if (scene) {
		//std::cout << "num of mNumMeshes : " << scene->mNumMeshes << std::endl;
		for( unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[i];

			meshes.emplace_back();
			auto & meshData = meshes[meshes.size() - 1];
			meshData.triangles.reserve(mesh->mNumFaces);

			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(scale));			

			for (unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];
				Vec3 vertex[3];
				Vec3 Normal[3];
				Vec3 uv[3];
                
                shapes.reserve(shapes.size() + face.mNumIndices);
				for (unsigned int k = 0; k < face.mNumIndices; k++) {
                    auto vertexId = face.mIndices[k];
					aiVector3D & v = mesh->mVertices[vertexId];
                    auto n = mesh->mNormals[vertexId];
                    auto normal = model * glm::vec4(n.x, n.y, n.z, 0.0);
					auto m = model * glm::vec4(v.x, v.y, v.z, 1.0);



                    auto uvData = mesh->mTextureCoords[0] != nullptr ? (mesh->mTextureCoords[0][vertexId]) : aiVector3D();
					uv[k] = Vec3(uvData.x, uvData.y, 0.0f);
					vertex[k] = {m.x, m.y, m.z};
					Normal[k] = {normal.x, normal.y, normal.z};
				}

				meshData.triangles.emplace_back();

				Triangle & triangle = meshData.triangles[meshData.triangles.size() - 1];
				triangle.Vertices[0] = vertex[0];
				triangle.Vertices[1] = vertex[1];
				triangle.Vertices[2] = vertex[2];
				triangle.normal[0] = Normal[0];
				triangle.normal[1] = Normal[1];
				triangle.normal[2] = Normal[2];
				triangle.uv[0] = uv[0]; triangle.uv[1] = uv[1]; triangle.uv[2] = uv[2];
				//triangle.uv[0] = {0.0f, 1.0f, 0.0f}; triangle.uv[1] = {1.0f, 0.0f, 0.0f}; triangle.uv[2] = {0.5f, 0.5f, 0.0f};
			}

			for (int i = 0; i < meshData.triangles.size(); ++i)
			{
				auto pTriangle = &meshData.triangles[i];
				shapes.push_back(pTriangle);
				shapeMaterialMap[pTriangle] = it->second;
			}


			//std::cout << "there's a mesh not triangle : " << mesh->mNumVertices << std::endl;

		}
	}

    std::cout << "Model : " << modelFile  << " Loaded ? : " << (scene != nullptr) << std::endl;

}



void Scene::AddMaterial(material * s) {
	materials.push_back(s);
	Materials.push_back(new PBMaterial());
}

void Scene::AddMaterial(std::string name, material * m) {
	materials.push_back(m);
    PBMaterial * mat = new PBMaterial();
    mat->color = glm::vec3(m->color.x, m->color.y, m->color.z);
    mat->emission = glm::vec3(m->emission.x, m->emission.y, m->emission.z);
    mat->metallic = m->metalic;
    //mat->roughness = 0.0f;
	Materials.push_back(mat);
	materialMap[name] = (int)materials.size()-1;
}

void Scene::AddMaterial(std::string name, Material * m)
{
	Materials.push_back(m);
	materials.push_back( new material({}, {}, 0) ); // TODO Fix
	materialMap[name] = (int)Materials.size()-1;
}


int Scene::GetShapeMaterialIdx(Shape * s) const {
	auto it = shapeMaterialMap.find(s);
	if (it == shapeMaterialMap.end()) {
		return -1;
	}

	return it->second;
}


void Scene::AddPointLight(glm::dvec3 position, glm::vec3 radiance, float radius)
{
	PointLight * pointLight = new PointLight();
	pointLight->position = position;
	pointLight->radiance = radiance;
	pointLight->radius = radius;
	lights.push_back(pointLight);
}

Texture* Scene::AddTexture(std::string texId, std::string path)
{
	std::cout << "Loading Texture : " << path << std::endl;

	Texture * tex = new Texture();
	bool bLoaded = LoadTexture(path, *tex);
	textures.push_back(tex);
	textureMap[texId] = textures.size()-1;
	return tex;
}

Texture* Scene::AddTexture(std::string texId, const Texture & texture)
{
	Texture * tex = new Texture();
	*tex = texture;
	textures.push_back(tex);
	textureMap[texId] = textures.size()-1;	
	return tex;
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

	build_bvh_SAH(tree, boundingBoxList);
	//std::cout << "size : " << tree.nodes.size() << std::endl;
	// build_bvh_simple(tree, boundingBoxList);
	//std::cout << "size : " << tree.nodes.size() << std::endl;
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


void MakeSceneData(const Scene & scene, SceneData & sceneData)
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
				const auto & v0 = triangleShape->Vertices[0];
				const auto & v1 = triangleShape->Vertices[1];
				const auto & v2 = triangleShape->Vertices[2];
				const auto & uv0 = triangleShape->uv[0];
				const auto & uv1 = triangleShape->uv[1];
				const auto & uv2 = triangleShape->uv[2];


				const auto & n0 = triangleShape->normal[0];
				const auto & n1 = triangleShape->normal[1];
				const auto & n2 = triangleShape->normal[2];


				int triangleIndex = AddShapesDataTriangle(sceneData.shapesData, 
					glm::dvec3(v0.x, v0.y, v0.z), glm::dvec3(v1.x, v1.y, v1.z), glm::dvec3(v2.x, v2.y, v2.z),
					glm::dvec3(n0.x, n0.y, n0.z), glm::dvec3(n1.x, n1.y, n1.z), glm::dvec3(n2.x, n2.y, n2.z),
					glm::dvec2(uv0.x, uv0.y), glm::dvec2(uv1.x, uv1.y), glm::dvec2(uv2.x, uv2.y)
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

	sceneData.textures = scene.textures;
}


bool IntersectScene(SceneData * sceneData, const BVHTree& bvhtree, const Ray3f & ray, float t_min, float t_max, SceneIntersectData & intersect)
{
	const int kTraceStackDepth = 128;
	int TraceStackData[kTraceStackDepth];
	bool bHitAny = BHV_Raycast(sceneData, bvhtree, ray, 0.1f, 10000.0f, intersect.point, intersect.normal, intersect.uv, intersect.shapeIdx, kTraceStackDepth, &TraceStackData[0]);

	return bHitAny;
}


bool EvalMaterialScatter(const Material & mat, const Ray3f & ray, const SceneIntersectData & intersect, HitInfo & hitInfo, Color & attenuation, Ray3f & scattered)
{
	SurfaceData surface;
	surface.position = intersect.point;
	surface.normal = intersect.normal;
	surface.uv = intersect.uv;
	return mat.scatter(ray, surface, hitInfo, attenuation, scattered);
}

