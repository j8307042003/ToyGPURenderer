#pragma once
#include "Application.h"
#include "AppWindow.h"
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>
#include <vector>
#include <unordered_map>

class InteractiveApp;
class PBMaterial;
class TestGUI : public ImguiUI
{

	virtual void OnGUI();

private:
	void MaterialPickGUI();
	void RayTestGUI();
	void SampleTestGUI();
	void CameraGUI();
private:
	float myColor[4];
	char saveFileBuffer[128];
public:
	Renderer* renderer;
	InteractiveApp* app;
	Camera* cam;

	PBMaterial* pMaterial = nullptr;

	glm::vec3 rayTestPose;
};

struct GLColorFormatResult
{
    unsigned int colorChannel;
    unsigned int format;
};


enum class AppEventType
{
	MouseClick
};

using EventCallback = std::function<void(int, int)>;

class InteractiveApp : public Application
{
public:
	InteractiveApp(char *argv[]);
	virtual ~InteractiveApp();
	virtual void Run();
	virtual void AddUI(ImguiUI * ui) override;
	virtual void RemoveUI(ImguiUI * ui) override;

public:
	float TimeStart() const {return m_timeStart;}
	float TimePass() const {return m_timePass;}

	bool GetControlLock() { return m_controlLock; }
	void SetControlLock(bool v) { m_controlLock = v;}
	void RegisterEvent(AppEventType eventType, EventCallback callback);

	Renderer* GetRenderer() { return m_renderer; }

private:
	void OnEvent(WindowEvent & event);
	void SignalCloseApp();
	void CameraUpdate(float deltaTime);
	void ViewportWindow(unsigned int textureId, int textureWidth, int textureHeight, void* frameBuffer, ColorFormat colorFormat, float deltaTime);
    	void ViewportWindowIO(float deltaTime, float imageUIWidth, float imageUIHeight, float textureWidth, float textureHeight);


public:
	bool bPickObj;
	bool bWaitPickObj;

private:
	AppWindow * m_appWindow;
	Renderer * m_renderer;
	Scene * m_scene;
	Camera * m_cam;
	std::vector<ImguiUI *> m_imguiUIs;
	bool m_key_table[512];
	bool m_mouse_table[32];
	bool m_prevMouseClicked = false;
	float m_mousePosX, m_mousePosY;
	float m_rotX, m_rotY;
	TestGUI m_testGUI;
	float m_timeStart;
	float m_timePass;
	bool m_controlLock = false;
    unsigned int m_viewportTextureId;
    float m_moveSpeed = 20.0f;
    
	std::unordered_map<AppEventType, std::vector<EventCallback>> m_listeners;

private:
	bool m_running;

friend class TestGUI;
};
