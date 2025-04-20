#include "Scene.h"
#include <typeinfo>
#include <map>
#include <string>


#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/gtx/transform.hpp"
#include "Material/PBMaterial.h"
#include "Material/EmissionMaterial.h"
//#include "Accelerate/BVHStruct.h"
#include "Env/EnvMapSource.h"
#include "RayTraceEngine/RayTraceEngine.h"
#include "RayTraceEngine/EmbreeEngine.h"
#include <stack>
#include <oneapi/tbb/parallel_for.h>
#include "Camera/petzval/petzval.h"
#include "Camera/petzval-kodak/petzval-kodak.h"
#include "Camera/canon-anamorphic/CanonAnamorphic.h"
#include "Camera/Camera.h"
#include <array>

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

inline void MakeOrthogonalCoordinateSystem(const glm::vec3 & v1, glm::vec3* v2, glm::vec3* v3)
{
	if (glm::abs(v1.x) > glm::abs(v1.y))
		*v2 = glm::vec3(-v1.z, 0, v1.x) * (1.0f / glm::sqrt(v1.x * v1.x + v1.z * v1.z));
	else
		*v2 = glm::vec3(0, v1.z, -v1.y) * (1.0f / glm::sqrt(v1.y * v1.y + v1.z * v1.z));
	*v3 = glm::cross(v1, *v2);

	*v2 = *v2 - glm::dot(v1, *v2) * v1;

}

bool Scene::LoadMeshResources(const std::string & path, ModelResource * & modelResource)
{
    aiPropertyStore* props = aiCreatePropertyStore();
    std::cout << "Loading Model : " << path << std::endl;


	//auto scene = aiImportFileExWithProperties(modelFile.c_str(), aiProcess_Triangulate, NULL, props);
	auto scene = aiImportFileExWithProperties(path.c_str(), aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_FlipUVs, NULL, props);
	//aiProcessPreset_TargetRealtime_Fast
	if (scene == nullptr)
		return false;

    
    auto start = std::chrono::high_resolution_clock::now();
    
	std::string directory = path.substr(0, path.find_last_of('/'));
    
	struct NodeTraversalData
	{
		aiMatrix4x4 transform;
		aiNode* node;
	};

	std::stack<NodeTraversalData> nodeStack = {};
	nodeStack.push({aiMatrix4x4(), scene->mRootNode});

	std::vector<NodeTraversalData> nodes = {};
	nodes.push_back({ scene->mRootNode->mTransformation, scene->mRootNode });
	while (nodeStack.size() > 0)
	{
		auto nodeData = nodeStack.top();
		nodeStack.pop();

		for (int c = 0; c < nodeData.node->mNumChildren; ++c)
		{
			auto child = nodeData.node->mChildren[c];
			auto transform = nodeData.transform * child->mTransformation;
			nodeStack.push({ transform , child });
			nodes.push_back({ transform , child });
		}
	}

	// Collect used texture
	std::map<std::string, std::string> texId_to_path = {};
	std::map<std::string, TextureWrapping> texId_to_wrapping = {};
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		auto p_material = scene->mMaterials[i];
		DumpMaterialTextures(p_material, directory, texId_to_path, texId_to_wrapping);
	}


	// Load texture at once
	std::vector<Texture*> textures = std::vector<Texture*>(texId_to_path.size());
	tbb::parallel_for(size_t(0), texId_to_path.size(), [&](size_t index)
	{
		auto it = texId_to_path.begin();
		std::advance(it, index);

		Texture* tex = new Texture();
		if (!LoadTexture(it->second, *tex)) {
			delete tex;
			return;
		}

		tex->wrapping = texId_to_wrapping[it->first];

		textures[index] = tex;
	}
	);

	for (int i = 0; i < textures.size(); ++i)
	{
		auto it = texId_to_path.begin();
		std::advance(it, i);
		AddTexture(it->first, it->second, textures[i]);
	}


    std::map<int, int> materialMap = {};
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
        auto p_material = scene->mMaterials[i];
	 	auto materialIndex = CreateMaterial(p_material, directory);
	 	materialMap.emplace(i, materialIndex);
	}



	modelResources.emplace_back();
	auto & resource = modelResources[modelResources.size() - 1];
    modelResource = &modelResources[modelResources.size() - 1];
    modelRecourcesMap[path] = modelResources.size() - 1;


	unsigned int vertexNum = 0;
    unsigned int indicsNum = 0;
    
    std::unordered_map<aiNode*, int> meshVertexStartIdxMap = {};
    std::unordered_map<aiNode*, int> meshIndicsStartIdxMap = {};
    
    for(int index = 0; index < nodes.size() ; ++index)
    {
        auto nodeData = nodes[index];
        
        meshVertexStartIdxMap[nodeData.node] = vertexNum;
        meshIndicsStartIdxMap[nodeData.node] = indicsNum;
        
        for (unsigned int i = 0; i < nodeData.node->mNumMeshes; ++i) {
            auto meshIdx = nodeData.node->mMeshes[i];
            aiMesh * pMesh = scene->mMeshes[meshIdx];
        
            vertexNum += pMesh->mNumVertices;
            indicsNum += pMesh->mNumFaces;
        }
    }
    
    
    resource.positions.resize(vertexNum);
    resource.normals.resize(vertexNum);
    resource.tangents.resize(vertexNum);
    resource.bitangents.resize(vertexNum);
    resource.uvs.resize(vertexNum);
    resource.materialIdx.resize(vertexNum);
    resource.triangles.resize(indicsNum);

    
	for(int index = 0; index < nodes.size() ; ++index)
	{
		auto nodeData = nodes[index];

		auto transform = nodeData.transform;
        
        auto vertexStart = meshVertexStartIdxMap[nodeData.node];
        auto indicsStart = meshIndicsStartIdxMap[nodeData.node];

        // set mesh data
		for (unsigned int i = 0; i < nodeData.node->mNumMeshes; ++i) {
            auto meshIdx = nodeData.node->mMeshes[i];
			aiMesh* mesh = scene->mMeshes[nodeData.node->mMeshes[i]];
            bool bHaveTangent = mesh->mTangents != nullptr;
            
            
            auto materialIdx = mesh->mMaterialIndex;
            int materialIndex = -1;
            if (mesh->mMaterialIndex >= 0)
            {
                auto materialPair = materialMap.find(mesh->mMaterialIndex);
                if (materialPair != materialMap.end())
                {
                    materialIndex = materialPair->second;
                }
            }
          
            // vertex data
            for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
            {
                aiVector3D world_pos = transform * mesh->mVertices[j];
                
                auto n = mesh->mNormals[j];
                aiVector3D t;
                if (bHaveTangent)
                {
                    t = mesh->mTangents[j];
                }
                else
                {
                    glm::vec3 glm_t, bt;
                    MakeOrthogonalCoordinateSystem({ n.x, n.y, n.z }, &glm_t, &bt);
                    t = { glm_t.x, glm_t.y, glm_t.z };
                }
                
                aiMatrix3x3 transformMat3 = aiMatrix3x3(transform);
                // TODO : Check normal
                t = transformMat3 * t;
                t = t.NormalizeSafe();

                n = transformMat3 * n;
                n = n.NormalizeSafe();

                auto normal = glm::vec4(n.x, n.y, n.z, 0.0);
                auto tangent = glm::vec4(t.x, t.y, t.z, 0.0);
                auto uvData = mesh->mTextureCoords[0] != nullptr ? (mesh->mTextureCoords[0][j]) : aiVector3D();
                
                resource.normals[vertexStart + j] = normal;
                resource.tangents[vertexStart + j] = tangent;
                resource.positions[vertexStart + j] = glm::vec3(world_pos.x, world_pos.y, world_pos.z);
                resource.uvs[vertexStart + j] = glm::vec2(uvData.x, uvData.y);
                resource.materialIdx[vertexStart + j] = materialIndex;
            }

            // indics
			for (unsigned int j = 0; j < mesh->mNumFaces; j++)
            {
                aiFace face = mesh->mFaces[j];
                
                resource.triangles[indicsStart + j] = glm::ivec3(vertexStart + face.mIndices[0], vertexStart + face.mIndices[1], vertexStart + face.mIndices[2]);
            }

            vertexStart += mesh->mNumVertices;
            indicsStart += mesh->mNumFaces;
		}
	}

    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    std::cout << path << " resource load time " << duration.count() << std::endl;
	return true;
}



void Scene::AddModel(std::string modelFile, std::string mat_name, Vec3 position, glm::quat rotation, float scale) {
	std::map<std::string,int>::iterator it = materialMap.find(mat_name);
	if (it == materialMap.end()) {
		std::cout << "Add Shape Failed. Due to add Shape with not existed material : " + mat_name << std::endl;
		return;
	}

	ModelResource* resource = nullptr;
    bool bLoaded = LoadMeshResources(modelFile, resource);

    if (!bLoaded) return;
    
    auto start = std::chrono::high_resolution_clock::now();
	int materialIndex = it->second;


	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
	model = glm::scale(model * toMat4(rotation), glm::vec3(scale));

    
    unsigned int shapeStartIdx = shapes.size();
    meshes.emplace_back();
    shapes.resize(shapes.size() + resource->triangles.size());
    auto & mesh = meshes[meshes.size() - 1];

    mesh.triangles.resize(resource->triangles.size());
    
    for (int i = 0; i < resource->triangles.size(); ++i)
    {
        auto & triangle = mesh.triangles[i];
        auto & resTri = resource->triangles[i];
        
        
        for (int j = 0; j < 3; ++j)
        {
            auto idx = resTri[j];
            auto & position = resource->positions[idx];
            auto & normal = resource->normals[idx];
            auto & tangent = resource->tangents[idx];
            auto & uv = resource->uvs[idx];
            
            glm::vec3 p = model * glm::vec4(position.x, position.y, position.z, 1.0f);
            glm::vec3 n = model * glm::vec4(normal.x, normal.y, normal.z, 0.0f);
            glm::vec3 t = model * glm::vec4(tangent.x, tangent.y, tangent.z, 0.0f);
            triangle.Vertices[j] = Vec3(p.x, p.y, p.z);
            triangle.normal[j] = Vec3(n.x, n.y, n.z);
            triangle.tangent[j] = Vec3(t.x, t.y, t.z);
            triangle.uv[j] = Vec3(uv.x, uv.y, 0.0f);
        }
        
        shapes[shapeStartIdx + i] = &mesh.triangles[i];
        shapeMaterialMap[&mesh.triangles[i]] = resource->materialIdx[resTri[0]];
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    std::cout << "add model instance Elapsed: " << duration.count() << " ms\n";
}


void Scene::AddEnv(IEnvSource * envSource)
{
	envSources.push_back(envSource);
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

inline TextureWrapping AssimpTexModeToTransform(aiTextureMapMode mapmode)
{
	switch (mapmode)
	{
		case aiTextureMapMode_Wrap: return TextureWrapping::Repeat;
		case aiTextureMapMode_Mirror: return TextureWrapping::Mirror;
		case aiTextureMapMode_Clamp: return TextureWrapping::Clamp;
	}
	return TextureWrapping::Repeat;
}

int Scene::CreateMaterial(aiMaterial* p_material, const std::string & filePath)
{

	PBMaterial* pbr_mat = new PBMaterial();
	pbr_mat->metallic = 0.0f;
	pbr_mat->roughness = 0.0f;
	//std::cout << p_material->GetName().C_Str() << std::endl;
	for (int i = 0; i < p_material->mNumProperties; ++i)
	{
		auto property = p_material->mProperties[i];
		auto data = property->mData;
		auto type = property->mType;

		aiString* pStr = nullptr;
		switch (type)
		{
		case aiPTI_Float: break;
		case aiPTI_String: pStr = reinterpret_cast<aiString*>(property->mData); break;
		}
		auto proKey = property->mKey.C_Str();
		float* value = reinterpret_cast <float*>(property->mData);
		//std::cout << "Property " << proKey << " " << *value << std::endl;
		if (strcmp(proKey, "$clr.diffuse") == 0)
		{
			float* diffuse = reinterpret_cast <float*>(property->mData);
			pbr_mat->color = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
		}
		else if (strcmp(proKey, "$mat.shininess") == 0)
		{
			float* metallic = reinterpret_cast <float*>(property->mData);
			//pbr_mat->metallic = metallic[0] / 1000.0f;
			pbr_mat->specularScale = metallic[0] / 1000.0f;
			if (pbr_mat->metallic > 0.9f)pbr_mat->color = glm::vec3(1.0f);
		}
		else if (strcmp(proKey, "$mat.metallicFactor") == 0)
		{
			float* metallic = reinterpret_cast <float*>(property->mData);
			pbr_mat->metallic = metallic[0];
		}		
		else if (strcmp(proKey, "$mat.roughnessFactor") == 0 )
		{
			float* roughness = reinterpret_cast <float*>(property->mData);
			pbr_mat->roughness = roughness[0];
		}		
	}
	//std::cout << std::endl;
	//pbr_mat->roughness = 0.0f;
	//pbr_mat->metallic = 0.0f;

    
    for(int texType = aiTextureType_NONE; texType <= AI_TEXTURE_TYPE_MAX; ++texType)
    {
        aiTextureType t = (aiTextureType) texType;
        int texCount = p_material->GetTextureCount((aiTextureType)texType);
        if (texCount == 0) continue;
        
        auto typeStr = TextureTypeToString(t);
        for (int texindex = 0; texindex < texCount; ++texindex)
        {
            aiString path;
            p_material->GetTexture(t, texindex, &path);
            //std::cout << "Type " << typeStr << " : " << path.C_Str() << std::endl;
        }
    }
    
    
	if (p_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		aiTextureMapMode mapmodes[2];
		p_material->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, mapmodes);
		//printf("Diffuse Texture: %s\n", path.C_Str());
		auto wrappingMode = AssimpTexModeToTransform(mapmodes[0]);
		auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()), wrappingMode);
		pbr_mat->albedo_texture = tex;
	}

	if (p_material->GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_SPECULAR, 0, &path);
		//printf("SPECULAR Texture: %s\n", path.C_Str());
	}

	if (p_material->GetTextureCount(aiTextureType_REFLECTION) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_REFLECTION, 0, &path);
		//printf("REFLECTION Texture: %s\n", path.C_Str());
	}

	if (p_material->GetTextureCount(aiTextureType_METALNESS) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_METALNESS, 0, &path);
		auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()));
		pbr_mat->metallic_texture = tex;
		//printf("METALNESS Texture: %s\n", path.C_Str());
	}
    
    if (p_material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
    {
        aiString path;
        p_material->GetTexture(aiTextureType_UNKNOWN, 0, &path);
        auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()));
        pbr_mat->metallic_texture = tex;
        pbr_mat->metallic_channel = 2;
        pbr_mat->roughness_texture = tex;
        pbr_mat->roughness_channel = 1;
        //printf("Unknown Texture: %s\n", path.C_Str());
    }
    

	if (p_material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
		//printf("Base Color Texture: %s\n", path.C_Str());
	}

	AddMaterial(std::string(p_material->GetName().C_Str()), pbr_mat);
	return Materials.size() - 1;
}


void Scene::DumpMaterialTextures(aiMaterial* p_material, const std::string& filePath, std::map<std::string, std::string>& map, std::map<std::string, TextureWrapping>& wrappingMap)
{
	if (p_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		aiTextureMapMode mapmodes[2];
		p_material->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, mapmodes);
		//printf("Diffuse Texture: %s\n", path.C_Str());
		auto wrappingMode = AssimpTexModeToTransform(mapmodes[0]);
		//auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()), wrappingMode);
		std::string texId = std::string(p_material->GetName().C_Str()) + std::string(path.C_Str());
		std::string texPath = filePath + "/" + std::string(path.C_Str());
		map[texId] = texPath;
		wrappingMap[texId] = wrappingMode;
	}

	if (p_material->GetTextureCount(aiTextureType_METALNESS) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_METALNESS, 0, &path);
		//auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()));
		std::string texId = std::string(p_material->GetName().C_Str()) + std::string(path.C_Str());
		std::string texPath = filePath + "/" + std::string(path.C_Str());
		map[texId] = texPath;
		wrappingMap[texId] = TextureWrapping();
	}

	if (p_material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
	{
		aiString path;
		p_material->GetTexture(aiTextureType_UNKNOWN, 0, &path);
		//auto tex = AddTexture(std::string(p_material->GetName().C_Str()) + std::string(path.C_Str()), filePath + "/" + std::string(path.C_Str()));
		std::string texId = std::string(p_material->GetName().C_Str()) + std::string(path.C_Str());
		std::string texPath = filePath + "/" + std::string(path.C_Str());
		map[texId] = texPath;
		wrappingMap[texId] = TextureWrapping();
	}
}



int Scene::GetShapeMaterialIdx(Shape * s) const {
	auto it = shapeMaterialMap.find(s);
	if (it == shapeMaterialMap.end()) {
		return -1;
	}

	return it->second;
}









// Structure for a vertex that holds position, UV, normal, and tangent.
struct Vertex {
    float x, y, z;     // Position
    float u, v;        // UV coordinates
    float nx, ny, nz;  // Normal
    float tx, ty, tz;  // Tangent
};

// Structure to represent a triangle by indices.
struct InnerTriangle {
    unsigned int v0, v1, v2;
};

// Helper function to normalize a 3D vector.
void normalize(float& x, float& y, float& z) {
    float length = std::sqrt(x * x + y * y + z * z);
    if (length > 0.00001f) {
        x /= length; y /= length; z /= length;
    }
}

// Generates vertices for a sphere with normals, tangents, and UVs.
std::vector<Vertex> generateSphereVertices(float radius, unsigned int rings, unsigned int sectors) {
    std::vector<Vertex> vertices;
    vertices.reserve(rings * sectors);

    // Loop over latitude rings.
    for (unsigned int r = 0; r < rings; ++r) {
        // phi is the polar angle (0 at the top pole and PI at the bottom).
        float phi = glm::pi<float>() * static_cast<float>(r) / static_cast<float>(rings - 1);
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        // Compute vertical texture coordinate.
        float vCoord = static_cast<float>(r) / static_cast<float>(rings - 1);

        // Loop over longitude sectors.
        for (unsigned int s = 0; s < sectors; ++s) {
            // theta is the azimuthal angle (0 to 2PI).
            float theta = 2 * glm::pi<float>() * static_cast<float>(s) / static_cast<float>(sectors - 1);
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            // Compute horizontal texture coordinate.
            float uCoord = static_cast<float>(s) / static_cast<float>(sectors - 1);

            // Compute vertex position using spherical coordinates.
            Vertex vtx;
            vtx.x = radius * sinPhi * cosTheta;
            vtx.y = radius * cosPhi;
            vtx.z = radius * sinPhi * sinTheta;

            // UV mapping.
            vtx.u = uCoord;
            vtx.v = vCoord;

            // Normal is the normalized position vector.
            vtx.nx = vtx.x;
            vtx.ny = vtx.y;
            vtx.nz = vtx.z;
            normalize(vtx.nx, vtx.ny, vtx.nz);

            // Tangent: derivative with respect to theta.
            // Note: At the poles (sinPhi ~ 0), we provide a default tangent.
            if (fabs(sinPhi) < 1e-5f) {
                // Default tangent (arbitrary) when sinPhi is nearly zero.
                vtx.tx = 1.0f;
                vtx.ty = 0.0f;
                vtx.tz = 0.0f;
            }
            else {
                // Partial derivative of position with respect to theta:
                // d/dθ (radius * sinPhi * cosTheta) = -radius * sinPhi * sinTheta
                // d/dθ (radius * sinPhi * sinTheta) = radius * sinPhi * cosTheta
                // d/dθ (radius * cosPhi) = 0
                vtx.tx = -radius * sinPhi * sinTheta;
                vtx.ty = 0.0f;
                vtx.tz = radius * sinPhi * cosTheta;
                normalize(vtx.tx, vtx.ty, vtx.tz);
            }

            vertices.push_back(vtx);
        }
    }
    return vertices;
}

// Generates triangle indices for the sphere mesh using rings and sectors.
std::vector<InnerTriangle> generateSphereIndices(unsigned int rings, unsigned int sectors) {
    std::vector<InnerTriangle> triangles;

    // Each small quad in the grid forms two triangles.
    for (unsigned int r = 0; r < rings - 1; ++r) {
        for (unsigned int s = 0; s < sectors - 1; ++s) {
            unsigned int current = r * sectors + s;
            unsigned int next = current + sectors;

            // First triangle of the quad.
            triangles.push_back({ current, next, current + 1 });
            // Second triangle of the quad.
            triangles.push_back({ current + 1, next, next + 1 });
        }
    }
    return triangles;
}














void Scene::AddPointLight(glm::dvec3 position, glm::vec3 radiance, float radius, bool visible)
{
	PointLight * pointLight = new PointLight();
	pointLight->position = position;
	pointLight->radiance = radiance;
	pointLight->radius = radius;
	pointLight->visible = visible;
	lights.push_back(pointLight);

	if (!visible)
	{
		return;
	}

	material * m = new material(Vec3(), Vec3(pointLight->radiance.x, pointLight->radiance.y, pointLight->radiance.z), 0.0);
	AddMaterial(m);

	EmissionMaterial * mat = new EmissionMaterial();
	mat->emission = radiance;
	Materials.push_back(mat);

	const int SPHERE_SEG = 15;
	std::vector<InnerTriangle> sphereMesh = generateSphereIndices(SPHERE_SEG, SPHERE_SEG);
	std::vector<Vertex> sphereVertices = generateSphereVertices(radius * 0.8f, SPHERE_SEG, SPHERE_SEG);
	meshes.resize(meshes.size() + 1);

	auto & mesh = meshes[meshes.size() - 1];
	mesh.triangles.resize(sphereMesh.size() * 3);


	int startIndex = shapes.size();
	shapes.resize(shapes.size() + sphereMesh.size());

	for (unsigned int i = 0; i < sphereMesh.size(); ++i)
	{
		auto & innerTri = sphereMesh[i];

		std::array<unsigned int, 3> tri = {innerTri.v0, innerTri.v1, innerTri.v2};
		auto pTriangle = &mesh.triangles[i * 3];
		shapes[startIndex + i] = pTriangle;

		for (int j = 0; j < tri.size(); ++j)
		{
			auto & v = sphereVertices[tri[j]];

			pTriangle->Vertices[j] = Vec3(v.x + position.x, v.y + position.y, v.z + position.z);
			pTriangle->uv[j] = Vec3(v.u, v.y, 0.0f);
			pTriangle->normal[j] = Vec3(v.nx, v.ny, v.nz);
			pTriangle->tangent[j] = Vec3(v.tx, v.ty, v.tz);

			shapeMaterialMap[pTriangle] = Materials.size() - 1;			
		}
	}

	std::cout << std::endl;
}


void Scene::AddSpotLight(glm::dvec3 position, glm::quat rotation, glm::vec3 radiance, float degree, float falloff)
{
	SpotLight * spotLight = new SpotLight();
	spotLight->position = position;
	spotLight->rotation = rotation;
	spotLight->radiance = radiance;
	spotLight->degree = degree;
	spotLight->falloff = std::min(degree, falloff);
	lights.push_back(spotLight);
}


void Scene::AddAreaLight(glm::dvec3 position, glm::quat rotation, glm::vec3 radiance, float width, float height, bool visible)
{
	AreaLight * areaLight = new AreaLight();
	areaLight->position = position;
	areaLight->rotation = rotation;
	areaLight->radiance = radiance;
	areaLight->width = width;
	areaLight->height = height;
	areaLight->visible = visible;
	lights.push_back(areaLight);

	if (!visible)
	{
		return;
	}




	material * m = new material(Vec3(), Vec3(areaLight->radiance.x, areaLight->radiance.y, areaLight->radiance.z), 0.0);
	AddMaterial(m);

	EmissionMaterial * mat = new EmissionMaterial();
	mat->emission = radiance;
	Materials.push_back(mat);

	std::array<glm::vec3, 4> glm_vertices = {
		glm::vec3(position) + rotation * glm::vec3( width,  height, -0.1),
		glm::vec3(position) + rotation * glm::vec3(-width,  height, -0.1),
		glm::vec3(position) + rotation * glm::vec3( width, -height, -0.1),
		glm::vec3(position) + rotation * glm::vec3(-width, -height, -0.1),
	};

	std::array<Vec3, 4> vertices = {
		Vec3(glm_vertices[0].x, glm_vertices[0].y, glm_vertices[0].z),
		Vec3(glm_vertices[1].x, glm_vertices[1].y, glm_vertices[1].z),
		Vec3(glm_vertices[2].x, glm_vertices[2].y, glm_vertices[2].z),
		Vec3(glm_vertices[3].x, glm_vertices[3].y, glm_vertices[3].z),
	};


	std::array<int, 3> tri_0_indics = {0, 1, 2};
	std::array<int, 3> tri_1_indics = {1, 2, 3};


	glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 tangent = rotation * glm::vec3(0.0f, 1.0f, 0.0f);


	meshes.resize(meshes.size() + 1);

	auto & mesh = meshes[meshes.size() - 1];
	mesh.triangles.resize(2);


	int startIndex = shapes.size();
	shapes.resize(shapes.size() + 2);

	auto tri_0 = &mesh.triangles[0];
	auto tri_1 = &mesh.triangles[1];
	shapeMaterialMap[tri_0] = Materials.size() - 1;	
	shapeMaterialMap[tri_1] = Materials.size() - 1;	


	shapes[startIndex + 0] = tri_0;
	for (int i = 0; i < 3; ++i)
	{
		tri_0->Vertices[i] = vertices[tri_0_indics[i]];
		tri_0->uv[i] = Vec3(0.0f, 0.0f, 0.0f); // lazy uv. TODO fix it
		tri_0->normal[i] = Vec3(forward.x, forward.y, forward.z);
		tri_0->tangent[i] = Vec3(tangent.x, tangent.y, tangent.z);
	}

	shapes[startIndex + 1] = tri_1;
	for (int i = 0; i < 3; ++i)
	{
		tri_1->Vertices[i] = vertices[tri_1_indics[i]];
		tri_1->uv[i] = Vec3(0.0f, 0.0f, 0.0f); // lazy uv. TODO fix it
		tri_1->normal[i] = Vec3(forward.x, forward.y, forward.z);
		tri_1->tangent[i] = Vec3(tangent.x, tangent.y, tangent.z);
	}

}



void Scene::AddDirectionalLight(glm::vec3 direction, glm::vec3 radiance)
{
	DirectionalLight * directionalLight = new DirectionalLight();
	directionalLight->direction = direction;
	directionalLight->radiance = radiance;

	lights.push_back(directionalLight);
}

void Scene::AddEnvSource(const std::string & path, float scale, float sampleScale)
{
	auto texptr = AddExrTexture(path, path);
	auto envMapSource = new EnvMapSource();
	envMapSource->envTexture = texptr;
	envMapSource->scale = scale;
	envMapSource->sampleScale = sampleScale;

	envSources.push_back(envMapSource);
	envSourceNames.push_back(path);
}

void Scene::AddEnvResource(const std::string & path)
{
	auto texptr = AddExrTexture(path, path);
}


Texture* Scene::AddTexture(std::string texId, std::string path)
{
	return AddTexture(texId, path, TextureWrapping::Clamp);
}

Texture* Scene::AddTexture(std::string texId, std::string path, TextureWrapping wrapping)
{
	// std::cout << "Loading Texture : " << path << std::endl;

	// search if texture already loaded
	auto search = textureFileMap.find(path);
	if (search != textureFileMap.end())
	{
		int textureIndex = search->second;
		textureMap[texId] = textureIndex;
		return textures[textureIndex];
	}

	Texture * tex = new Texture();
	if (!LoadTexture(path, *tex)) {
		delete tex;
		return nullptr;
	}
	tex->wrapping = wrapping;
	textures.push_back(tex);
	textureMap[texId] = textures.size()-1;
	textureFileMap[path] = textures.size() - 1;
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

Texture* Scene::AddTexture(std::string texId, std::string path, Texture* texture)
{
	textures.push_back(texture);
	textureMap[texId] = textures.size() - 1;
	textureFileMap[path] = textures.size() - 1;
	return texture;
}

Texture* Scene::AddExrTexture(const std::string & texId, const std::string & path)
{
	Texture * tex = new Texture();	
	if (!LoadExrTexture(path, *tex))
	{
		delete tex;
		return nullptr;
	}

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


void MakeSceneData(const Scene & scene, SceneData & sceneData, bool enableEmbree)
{
	if (scene.cameraModel.compare("petzval")== 0)
	{
        sceneData.cameraData = petzval_camera_data::MakeCamData();
	}
    else if (scene.cameraModel.compare("petzval-kodak") == 0)
    {
        sceneData.cameraData = petzval_kodak_camera_data::MakeCamData();
    }
    else if (scene.cameraModel.compare("Canon-Anamorphic") == 0)
    {
		sceneData.cameraData = CanonAnamorphic_camera_data::MakeCamData();
    }
    else
    {
        sceneData.cameraData = DefaultCameraData();
    }


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

				const auto & t0 = triangleShape->tangent[0];
				const auto & t1 = triangleShape->tangent[1];
				const auto & t2 = triangleShape->tangent[2];


				int triangleIndex = AddShapesDataTriangle(sceneData.shapesData, 
					glm::dvec3(v0.x, v0.y, v0.z), glm::dvec3(v1.x, v1.y, v1.z), glm::dvec3(v2.x, v2.y, v2.z),
					glm::dvec3(n0.x, n0.y, n0.z), glm::dvec3(n1.x, n1.y, n1.z), glm::dvec3(n2.x, n2.y, n2.z),
					glm::dvec3(t0.x, t0.y, t0.z), glm::dvec3(t1.x, t1.y, t1.z), glm::dvec3(t2.x, t2.y, t2.z),
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

	sceneData.envSources = scene.envSources;
	sceneData.envSourceNames = scene.envSourceNames;

	sceneData.textures = scene.textures;

	if (enableEmbree)
	{
		sceneData.pRayTraceEngine = EmbreeEngine::BuildEmgreeEngine(&sceneData);
	}
	else
	{
		sceneData.bvh_tree = new BVHTree();
		bvh_buildTree1(&sceneData, *sceneData.bvh_tree);	
		sceneData.pRayTraceEngine = new RayTraceEngine();
	}
}


bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData & intersect)
{
	/*
	const int kTraceStackDepth = 128;
	int TraceStackData[kTraceStackDepth];
	bool bHitAny = BHV_Raycast(sceneData, bvhtree, ray, t_min, t_max, intersect.point, intersect.normal, intersect.uv, intersect.shapeIdx, kTraceStackDepth, &TraceStackData[0]);
	return bHitAny;
	*/

	return sceneData->pRayTraceEngine->IntersectScene(sceneData, ray, t_min, t_max, &intersect);
}

bool IntersectScene(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max, int* stackBuffer, int stackSize, SceneIntersectData& intersect)
{
	//bool bHitAny = BHV_Raycast(sceneData, bvhtree, ray, t_min, t_max, intersect.point, intersect.normal, intersect.uv, intersect.shapeIdx, stackSize, stackBuffer);
	//return bHitAny;
	return false;
}

bool OccuScene(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max)
{
	return sceneData->pRayTraceEngine->Raycast(sceneData, ray, t_min, t_max);
}



bool EvalMaterialScatter(const Material & mat, const glm::vec3& view, const glm::vec3 & wi, const SceneIntersectData & intersect, /*HitInfo & hitInfo,*/ Color & attenuation/*, Ray3f & scattered*/)
{
	auto bitangent = glm::cross(intersect.normal, intersect.tangent);
	glm::mat3 tangentToWorld = glm::mat3(intersect.tangent, intersect.normal, bitangent);

	SurfaceData surface;
	surface.position = intersect.point;
	surface.normal = intersect.normal;
	surface.tangent = intersect.tangent;
	surface.bitangent = bitangent;
	surface.worldToTangent = glm::transpose(tangentToWorld);
	surface.uv = intersect.uv;

	return mat.scatter(view, wi, surface, /*hitInfo,*/ attenuation/*, scattered*/);
}

bool EvalMaterialBRDF(const Material & mat, const Ray3f & ray, const SceneIntersectData & intersect, BsdfSample & bsdfSample)
{
	auto bitangent = glm::cross(intersect.normal, intersect.tangent);
	glm::mat3 tangentToWorld = glm::mat3(intersect.tangent, intersect.normal, bitangent);
	
	SurfaceData surface;
	surface.position = intersect.point;
	surface.normal = intersect.normal;
	surface.tangent = intersect.tangent;
	surface.bitangent = bitangent;
	surface.worldToTangent = glm::transpose(tangentToWorld);
	surface.uv = intersect.uv;
	return mat.sampleBsdf(surface, ray, bsdfSample);
}

glm::vec3 EvalMaterialEmission(const Material& mat, const SceneIntersectData& intersect)
{
	auto bitangent = glm::cross(intersect.normal, intersect.tangent);
	glm::mat3 tangentToWorld = glm::mat3(intersect.tangent, intersect.normal, bitangent);

	SurfaceData surface;
	surface.position = intersect.point;
	surface.normal = intersect.normal;
	surface.tangent = intersect.tangent;
	surface.bitangent = bitangent;
	surface.worldToTangent = glm::transpose(tangentToWorld);
	surface.uv = intersect.uv;
	return mat.Emission(surface);
}


void update_dynamic_obj_mesh(DynamicMesh* mesh)
{
	glm::quat quat = glm::quat(glm::radians(mesh->eulerRotation));

	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, mesh->position);
	model = glm::scale(model * toMat4(quat), glm::vec3(mesh->scale));

	ModelResource * resource = mesh->resources;
	for (int i = 0; i < mesh->vertices.size(); ++i)
	{
		mesh->vertices[i] = model * glm::vec4(resource->positions[i], 1.0f);
		mesh->normals[i] = model * glm::vec4(resource->normals[i], 0.0f);
		mesh->tangents[i] = model * glm::vec4(resource->tangents[i], 0.0f);
	}

}

bool UpdateDynamicsObj(SceneData * sceneData, DynamicMesh * obj)
{
	update_dynamic_obj_mesh(obj);

 	if (sceneData->pRayTraceEngine == nullptr) return false;
	sceneData->pRayTraceEngine->UpdateDynamicGeometry(obj);   	
}


bool AddDynamicsObj(SceneData * sceneData, ModelResource* resource, const char* name)
{
    sceneData->dynamicObjs.emplace_back();
    
    DynamicMesh & obj = sceneData->dynamicObjs[sceneData->dynamicObjs.size() - 1];
    obj.resources = resource;
    obj.position = glm::vec3();
    obj.eulerRotation = glm::vec3();
    obj.scale = 1.0f;
    obj.name = name;
    obj.vertices.resize(resource->positions.size());
    obj.normals.resize(resource->normals.size());
    obj.tangents.resize(resource->tangents.size());


    update_dynamic_obj_mesh(&obj);
 
 	if (sceneData->pRayTraceEngine == nullptr) return false;
	sceneData->pRayTraceEngine->AddDynamicGeometry(&obj);   

	return true; 
}

