#include "InteractiveApp.h"
#include "AppWindowGLFW.h"
#include <GLFW/glfw3.h>
#include "Renderer/VulkanRenderer.h"
#include "Renderer/TestScene1.h"

InteractiveApp::InteractiveApp(const char * args)
{
	m_appWindow = new AppWindowGLFW();
	m_renderer = new VulkanRenderer();
	m_scene = make_test_scene1();
	m_scene->BuildTree();
	m_cam = new Camera(512, 512, Vec3(200.0f, 0.0f, -200.0f));

	m_renderer->SetRenderData(m_scene, m_cam);
}

void InteractiveApp::Run()
{
	float time = (float)glfwGetTime();
	m_running = true;
	m_renderer->StartRender();

	while(m_running)
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - time;
		time = currentTime;

		m_renderer->UpdateFrame();
		m_appWindow->SetSourceImage(m_cam->GetWidth(), m_cam->GetHeight(), (char*)m_cam->GetBuffer());
		m_appWindow->Update();
	}
}
