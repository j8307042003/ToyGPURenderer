#pragma once
#include "../Camera/Camera.h"
#include "../Scene.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct RenderData
{
	// Camera
	CameraData camData;
	glm::dvec3 camPosition;
	glm::dvec3 camDirection;
	
	// Scene Geometry
	Scene* scene;
	SceneData* sceneData;
	// Resolution

};

struct TileData
{
	int x;
	int y;

	int width;
	int height;
};

class IRenderMethod
{
public:
	virtual ~IRenderMethod() = default;
	virtual glm::vec3 Sample(const RenderData & rdrData, int x, int y, glm::vec2 filmRes) = 0;
};
