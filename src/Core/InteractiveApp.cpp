#include <functional>
#include "InteractiveApp.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/ParallelRenderer.h"
#include "Renderer/TestScene1.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "AppWindowGLFW.h"
InteractiveApp::InteractiveApp(const char * args)
{
	m_appWindow = new AppWindowGLFW();
	auto f = std::bind(&InteractiveApp::OnEvent, this, std::placeholders::_1);
	m_appWindow->SetEventCallback(f);
	// m_renderer = new VulkanRenderer();
	m_renderer = new ParallelRenderer();
	m_scene = make_test_scene1();
	m_scene->BuildTree();
	m_cam = new Camera(512, 512, Vec3(200.0f, 0.0f, -200.0f));

	m_renderer->SetRenderData(m_scene, m_cam);
    

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    const char* glsl_version = "#version 150";
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_appWindow->GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(nullptr);
}

InteractiveApp::~InteractiveApp()
{
	delete m_appWindow;
	delete m_renderer;
	delete m_scene;
	delete m_cam;
}


void InteractiveApp::OnEvent(WindowEvent & event)
{
	if (event.GetEventType() == EWindowEvent::KeyPressed)
	{
		std::cout << "Key pressed!!" << std::endl;
		KeyPressedEvent keyEvent = dynamic_cast<KeyPressedEvent&>(event);
		if (keyEvent.keyCode == GLFW_KEY_ESCAPE)
			SignalCloseApp();
	}
}

void InteractiveApp::SignalCloseApp()
{
	m_running = false;
}



void InteractiveApp::Run()
{
	float time = (float)glfwGetTime();
	m_running = true;
	m_renderer->StartRender();
    float myColor[4];
	while(m_running)
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - time;
		time = currentTime;
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        bool bShow = true;
        ImGui::ShowDemoWindow(&bShow);
        
        ImGui::Begin("Test imgui window");
        ImGui::ColorEdit4("Color", myColor);
        ImGui::End();
        ImGui::EndFrame();
        
		m_renderer->UpdateFrame();
		m_appWindow->SetSourceImage(m_cam->GetWidth(), m_cam->GetHeight(), (char*)m_cam->GetBuffer());
		m_appWindow->Update();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_appWindow->SwapBuffer();
	}
}
