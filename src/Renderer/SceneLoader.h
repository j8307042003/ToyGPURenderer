#pragma once
#include "Scene.h"
#include <json11.hpp>

class SceneLoader {
public:
	static void Load(const std::string & path, Scene & scene);

private:
	static void LoadEnvData(const json11::Json& envJsonData, Scene* scene);
	static void LoadMeshData(const json11::Json& sceneJson, Scene* scene);
	static void LoadMaterial(const json11::Json& materialJson, Scene* scene);
	static void LoadLight(const json11::Json& lightJson, Scene* scene);
};