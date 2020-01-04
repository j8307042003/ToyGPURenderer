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
	int itNum = 0;
	void StartTask();
	void EndTask();

	void RenderTask();
	void SplitRender(int x_start, int y_start, int x_end, int y_end,  int offset, int threadNum);

};