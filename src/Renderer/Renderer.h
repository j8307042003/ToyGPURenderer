#pragma once

#include <thread>
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
	Renderer(){};

	void SetRenderData(Scene * s, Camera * cam);
	void StartRender();
private:
	bool stopFlag;
	Scene * s;
	Camera * cam;
	std::thread renderThread;

	void StartTask();
	void EndTask();

	void RenderTask();
};