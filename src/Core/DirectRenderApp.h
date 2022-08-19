#pragma once
#include "Application.h"
#include "AppWindow.h"
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>
#include <vector>
#include "Renderer/PathTraceRenderer.h"

class DirectRenderApp : public Application
{
public:
	DirectRenderApp(char* argv[]);
	virtual ~DirectRenderApp();
	virtual void Run();
	virtual void AddUI(ImguiUI * ui) override {}
	virtual void RemoveUI(ImguiUI* ui) override {}

private:
	void SignalCloseApp();

private:
	PathTraceRenderer * m_renderer;
	Scene * m_scene;
	Camera * m_cam;
	float m_timeStart;
	float m_timePass;

private:
	bool m_running;
};
