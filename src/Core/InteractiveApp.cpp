#include <functional>
#include "InteractiveApp.h"
#include <glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "AppWindowGLFW.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/ParallelRenderer.h"
#include "Renderer/TestScene1.h"
#include "Renderer/PathTraceRenderer.h"


InteractiveApp::InteractiveApp(const char * args) : m_running(false)
{
	const int kWidth = 1920 / 2;//720;
	const int kHeight = 1080 / 2;//512;
	m_appWindow = new AppWindowGLFW(kWidth, kHeight);
	auto f = std::bind(&InteractiveApp::OnEvent, this, std::placeholders::_1);
	m_appWindow->SetEventCallback(f);
	// m_renderer = new VulkanRenderer();
	// m_renderer = new ParallelRenderer();
	m_renderer = new PathTraceRenderer();
	m_scene = make_test_scene1();
	//m_scene->BuildTree();
	m_cam = new Camera(kWidth, kHeight, Vec3(200.0f, 0.0f, -200.0f));

	m_renderer->SetRenderData(m_scene, m_cam);
    

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    const char* glsl_version = "#version 150";
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_appWindow->GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init(nullptr);

	m_testGUI = {};
	m_testGUI.renderer = m_renderer;
	m_testGUI.app = this;
	AddUI(&m_testGUI);
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
		else if (keyEvent.keyCode == GLFW_KEY_W)
		{

			m_renderer->ClearImage();
		}
		else if (keyEvent.keyCode == GLFW_KEY_S)
		{

			m_renderer->ClearImage();
		}
		else if (keyEvent.keyCode == GLFW_KEY_A)
		{

			m_renderer->ClearImage();
		}
		else if (keyEvent.keyCode == GLFW_KEY_D)
		{

			m_renderer->ClearImage();
		}
	}
}


void InteractiveApp::AddUI(ImguiUI * ui)
{
	bool bExist = std::find(m_imguiUIs.begin(), m_imguiUIs.end(), ui) != m_imguiUIs.end();
	if (!bExist) m_imguiUIs.push_back(ui);
}


void InteractiveApp::RemoveUI(ImguiUI * ui)
{
	auto it = std::find(m_imguiUIs.begin(), m_imguiUIs.end(), ui);
	if (it == m_imguiUIs.end()) return;
	m_imguiUIs.erase(it);
}


void InteractiveApp::SignalCloseApp()
{
	m_running = false;
}

void TestGUI::OnGUI()
{
	ImGui::Begin("Test imgui window");

	PathTraceRenderer* pathTraceRenderer = (PathTraceRenderer*)renderer;

	int renderItProgress = pathTraceRenderer->Iteration();
	ImGui::Text("Iteration : %d", renderItProgress);

	float timeStart = app->TimeStart();
	float timePass = app->TimePass();
	ImGui::Text("TimePass : %f", timePass);
	ImGui::Text("iterate per second: %f", renderItProgress / timePass);

	bool showDenoise = pathTraceRenderer->GetShowDenoise();
	ImGui::Checkbox("Denoise", &showDenoise);
	pathTraceRenderer->SetShowDenoise(showDenoise);

	ImGui::End();
}




void InteractiveApp::Run()
{
	float time = (float)glfwGetTime();
	m_timeStart = time;	
	m_running = true;
	m_renderer->StartRender();
    float myColor[4];
	while(m_running)
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - time;
		time = currentTime;
		m_timePass = currentTime - m_timeStart;
        
        // bool bShow = true;
        // ImGui::ShowDemoWindow(&bShow);
        
		m_renderer->UpdateFrame();
		void* pBuffer = m_renderer->GetImage();
		if (pBuffer != nullptr) m_appWindow->SetSourceImage(m_cam->GetWidth(), m_cam->GetHeight(), (char*)pBuffer, ColorFormat::RGBByte);
		m_appWindow->Update();
        
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        for(int i = 0; i < m_imguiUIs.size(); ++i)
        {
        	m_imguiUIs[i]->OnGUI();
        }        

		ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
        m_appWindow->SwapBuffer();
	}
}
