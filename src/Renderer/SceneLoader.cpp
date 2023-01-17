#include "SceneLoader.h"
#include <map>
#include <fstream>      // std::ifstream
#include <memory>
#include <Renderer/Material/PBMaterial.h>

void SceneLoader::Load(const std::string & path, Scene & scene)
{
	// Open File
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	// Load Buffer
	size_t 	filesize = (size_t)file.tellg();
	std::vector<char> buffer(filesize + 1);
	file.seekg(0);
	file.read(buffer.data(), filesize);
	file.close();
	buffer[buffer.size() - 1] = '\0';


	std::string err;
	auto sceneJsonData = json11::Json::parse(buffer.data(), err);

	auto materialJsonData = sceneJsonData["materials"];
	LoadMaterial(materialJsonData, &scene);

	auto envJsonData = sceneJsonData["Env"];
	LoadEnvData(envJsonData, &scene);

	auto lightJsonData = sceneJsonData["Light"];
	LoadLight(lightJsonData, &scene);

	LoadMeshData(sceneJsonData, &scene);
}


void SceneLoader::LoadEnvData(const json11::Json& envJsonData, Scene* scene)
{
	if (!envJsonData.is_array())
	{
		return;
	}

	auto envJsonArrayDatas = envJsonData.array_items();
	for (int i = 0; i < envJsonArrayDatas.size(); ++i)
	{
		auto envJson = envJsonArrayDatas[i];
		auto typestring = envJson["type"].string_value();

		if (typestring == "EnvMap")
		{
			auto mapFilePath = envJson["file"].string_value();
			auto scale = envJson["scale"].number_value();
			auto sampleScale = envJson["sampleScale"].number_value();
			scene->AddEnvSource(mapFilePath, scale);
		}
	}

}


//Vec3 position, glm::quat rotation, float scale

Vec3 ArrayToVec3(json11::Json j)
{
	auto data = j.array_items();
	return Vec3(data[0].number_value(), data[1].number_value(), data[2].number_value());
}

glm::quat ArrayToQuat(json11::Json j)
{
	auto data = j.array_items();
	return glm::quat({glm::radians(data[0].number_value()),glm::radians(data[1].number_value()), glm::radians(data[2].number_value()) });
}

void SceneLoader::LoadMeshData(const json11::Json& sceneJson, Scene* scene)
{
	std::map<std::string, std::string> meshFileMap = {};

	auto meshsJson = sceneJson["meshs"];
	auto meshDatas = meshsJson.array_items();

	for (int i = 0; i < meshDatas.size(); ++i)
	{
		auto meshData = meshDatas[i];
		auto name = meshData["name"].string_value();
		auto file = meshData["file"].string_value();
		meshFileMap[name] = file;
	}

	auto meshInstanceJson = sceneJson["models"];
	auto meshInstanceDatas = meshInstanceJson.array_items();

	for (int i = 0; i < meshInstanceDatas.size(); ++i)
	{
		auto meshInstanceDataJson = meshInstanceDatas[i];
		auto mesh = meshInstanceDataJson["mesh"].string_value();
		auto material = meshInstanceDataJson["material"].string_value();
		auto pos = meshInstanceDataJson["pos"];
		auto rot = meshInstanceDataJson["rot"];
		auto scale = meshInstanceDataJson["scale"].number_value();

		auto posData = ArrayToVec3(pos);
		auto rotData = ArrayToQuat(rot);
		auto search = meshFileMap.find(mesh);
		if (search == meshFileMap.end()) continue;
		auto modelPath = search->second;

		scene->AddModel(modelPath, material, posData, rotData, scale);
	}
}

void SceneLoader::LoadMaterial(const json11::Json& materialJson, Scene* scene)
{
	auto materialJsonDatas = materialJson.array_items();

	for (int i = 0; i < materialJsonDatas.size(); ++i)
	{
		auto materialData = materialJsonDatas[i];
		auto type = materialData["type"].string_value();
		auto name = materialData["name"].string_value();

		if (type == "PBMaterial")
		{
			PBMaterial* pMat = new PBMaterial();
			scene->AddMaterial(name, pMat);
		}
	}
}

void SceneLoader::LoadLight(const json11::Json& lightJson, Scene* scene)
{
	auto lightJsonDatas = lightJson.array_items();

	for (int i = 0; i < lightJsonDatas.size(); ++i)
	{
		auto lightData = lightJsonDatas[i];
		auto type = lightData["type"].string_value();

		if (type == "point")
		{
			auto radius = lightData["radius"].number_value();
			auto pos = lightData["pos"];
			auto radiance = lightData["radiance"];

			auto posData = ArrayToVec3(pos);
			auto radianceData = ArrayToVec3(radiance);

			scene->AddPointLight(glm::dvec3(posData.x, posData.y, posData.z), glm::dvec3(radianceData.x, radianceData.y, radianceData.z), radius);
		}
	}
}
