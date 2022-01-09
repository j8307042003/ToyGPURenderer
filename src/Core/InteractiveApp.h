#pragma once
#include "Application.h"
#include "AppWindow.h"
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>
#include <vector>


class TestGUI : public ImguiUI
{

	virtual void OnGUI();
private:
	float myColor[4];
public:
	Renderer* renderer;
};


class InteractiveApp : public Application
{
public:
	InteractiveApp(const char * args);
	virtual ~InteractiveApp();
	virtual void Run();
	virtual void AddUI(ImguiUI * ui) override;
	virtual void RemoveUI(ImguiUI * ui) override;

private:
	void OnEvent(WindowEvent & event);
	void SignalCloseApp();

private:
	AppWindow * m_appWindow;
	Renderer * m_renderer;
	Scene * m_scene;
	Camera * m_cam;
	std::vector<ImguiUI *> m_imguiUIs;
	TestGUI m_testGUI;

private:
	bool m_running;
};
