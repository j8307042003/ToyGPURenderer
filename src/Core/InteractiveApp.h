#pragma once
#include "Application.h"
#include "AppWindow.h"
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>
#include <vector>

class InteractiveApp;
class TestGUI : public ImguiUI
{

	virtual void OnGUI();
private:
	float myColor[4];
public:
	Renderer* renderer;
	InteractiveApp* app;
};


class InteractiveApp : public Application
{
public:
	InteractiveApp(const char * args);
	virtual ~InteractiveApp();
	virtual void Run();
	virtual void AddUI(ImguiUI * ui) override;
	virtual void RemoveUI(ImguiUI * ui) override;

public:
	float TimeStart() const {return m_timeStart;}
	float TimePass() const {return m_timePass;}

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
	float m_timeStart;
	float m_timePass;

private:
	bool m_running;
};
