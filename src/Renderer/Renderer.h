#pragma once

#include <thread>
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
	Renderer(){};

	void SetRenderData(Scene * s, Camera * cam);
	virtual void StartRender() = 0;
	virtual void UpdateFrame() = 0;
	virtual void ClearImage() = 0;
protected:
	Scene * s;
	Camera * cam;
};