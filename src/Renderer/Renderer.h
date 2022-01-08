#pragma once

#include <thread>
#include "Scene.h"
#include "Camera.h"

class Renderer {
public:
	Renderer(){};
	virtual ~Renderer() = default;

	void SetRenderData(Scene * s, Camera * cam);
	virtual void StartRender() = 0;
	virtual void UpdateFrame() = 0;
	virtual void ClearImage() = 0;
	virtual void Stall() {running = false;}
	virtual void Resume() {running = true;}
	virtual void* GetImage() { return nullptr; }
	
protected:
	Scene * s;
	Camera * cam;

	bool running = true;
};