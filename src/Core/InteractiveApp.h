#pragma once
#include "Application.h"
#include "AppWindow.h"
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>

class InteractiveApp : public Application
{
public:
	InteractiveApp(const char * args);
	virtual void Run();

private:
	AppWindow * m_appWindow;
	Renderer * m_renderer;
	Scene * m_scene;
	Camera * m_cam;

private:
	bool m_running;
};