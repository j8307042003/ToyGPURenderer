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
#include <glm/ext/quaternion_common.hpp>
#include <fstream>       //���Jfstream���Y��
#include <ctime>
#include <sstream>
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#endif // !STB_IMAGE_WRITE_IMPLEMENTATION
#include "Common/stb_image_write.h"
#include "Renderer/SceneLoader.h"
#include <embree3/rtcore.h>

InteractiveApp::InteractiveApp(char *argv[]) : m_running(false), m_key_table(), m_mouse_table(), m_imguiUIs()
{
	const int kWidth = 2560; //1920 / 1;// 1920 / 1;//720;
	const int kHeight = 1440;//1080 / 1;// 1080 / 1;//512;
	m_appWindow = new AppWindowGLFW(kWidth, kHeight);
	auto f = std::bind(&InteractiveApp::OnEvent, this, std::placeholders::_1);
	m_appWindow->SetEventCallback(f);
	// m_renderer = new VulkanRenderer();
	// m_renderer = new ParallelRenderer();
	m_renderer = new PathTraceRenderer();


	m_scene = new Scene();
	SceneLoader::Load("scene/scene1.json", *m_scene);
	//m_scene = make_test_scene1();


	//m_scene->BuildTree();
	m_cam = new Camera(kWidth, kHeight, Vec3(200.0f, 0.0f, -200.0f));
	//m_cam->rotation = glm::quatLookAt(glm::vec3(0.0f, 0.4f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_cam->rotation = glm::quatLookAt(glm::vec3(0.0f, 0.3f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_cam->rotation = glm::quat(glm::vec3(2.82,-0.10,0.00));
	auto angles = glm::eulerAngles(m_cam->rotation);
	m_rotX = angles.y;
	m_rotY = angles.x;
	//m_cam->pos = glm::dvec3(0, 0, 10);
	m_cam->pos = glm::dvec3(0.0f, 10.81f, 37.72f);
	//m_cam->pos = glm::dvec3(-148.99, 73.11f, 196.40f); // for minecraft

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
	m_testGUI.cam = m_cam;
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
	auto event_type = event.GetEventType();

	if (event_type == EWindowEvent::KeyPressed)
	{
		KeyPressedEvent keyEvent = dynamic_cast<KeyPressedEvent&>(event);
		m_key_table[keyEvent.keyCode] = true;
	}
	else if (event_type == EWindowEvent::KeyReleased)
	{
		KeyReleaseEvent keyEvent = dynamic_cast<KeyReleaseEvent&>(event);
		m_key_table[keyEvent.keyCode] = false;

		if (bWaitPickObj)
		{
			bWaitPickObj = false;
			bPickObj = true;
		}
	}

	if (event.GetEventType() == EWindowEvent::KeyPressed)
	{
		std::cout << "Key pressed!!" << std::endl;
		KeyPressedEvent keyEvent = dynamic_cast<KeyPressedEvent&>(event);
		if (keyEvent.keyCode == GLFW_KEY_ESCAPE)
			SignalCloseApp();
		// else if (keyEvent.keyCode == GLFW_KEY_W)
		// {
		// 	m_renderer->ClearImage();
		// }
		// else if (keyEvent.keyCode == GLFW_KEY_S)
		// {
		// 	m_renderer->ClearImage();
		// }
		// else if (keyEvent.keyCode == GLFW_KEY_A)
		// {
		// 	m_renderer->ClearImage();
		// }
		// else if (keyEvent.keyCode == GLFW_KEY_D)
		// {
		// 	m_renderer->ClearImage();
		// }
	
	}

	if (event.GetEventType() == EWindowEvent::MousePressed)
	{
		MousePressedEvent keyEvent = dynamic_cast<MousePressedEvent&>(event);
		m_mouse_table[keyEvent.mouseCode] = true;
	}
	else if (event.GetEventType() == EWindowEvent::MouseReleased)
	{
		MouseReleaseEvent keyEvent = dynamic_cast<MouseReleaseEvent&>(event);
		m_mouse_table[keyEvent.mouseCode] = false;
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

void RenderUI_Camera(Camera* cam)
{
	const auto pos = cam->pos;
	ImGui::Text("Camera Position %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);

	const auto rotation = glm::eulerAngles(cam->rotation);
	ImGui::Text("Camera rotation %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);
}


using sysclock_t = std::chrono::system_clock;

std::string CurrentDate()
{
	std::time_t now = sysclock_t::to_time_t(sysclock_t::now());

	char buf[16] = { 0 };
	std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));

	return std::string(buf);
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

	bool controlLock = app->GetControlLock();
	ImGui::Checkbox("Move Lock", &controlLock);
	app->SetControlLock(controlLock);

	auto displayKind = pathTraceRenderer->GetShowDisplayChannel();

	const char* DisplayLabelText[] = {
		"RawImage",
		"Denoised",
		"Albedo",
		"Normal",
		"SimpleShading"
	};

	int idx =  PathTraceRenderer::DisplayChannelToInt(displayKind);
	static const char* current_item = DisplayLabelText[idx];
	if (ImGui::BeginCombo("Display Channel", current_item))
	{
		for (int n = 0; n < IM_ARRAYSIZE(DisplayLabelText); n++)
		{
			bool is_selected = (current_item == DisplayLabelText[n]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(DisplayLabelText[n], is_selected))
			{
				current_item = DisplayLabelText[n];
				idx = n;
				pathTraceRenderer->SetShowDisplayChannel(PathTraceRenderer::IntToDisplayChannel(idx));
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}	

	ImGui::InputText("Save Pic Name", saveFileBuffer, 128);

	if (ImGui::Button("Save Image"))
	{
		unsigned char *  ptr_image = (unsigned char*)pathTraceRenderer->GetImage();
		auto height = pathTraceRenderer->GetHeight();
		auto width = pathTraceRenderer->GetWidth();
		stbi_flip_vertically_on_write(1);

		std::string fileName = std::string(saveFileBuffer) + ".png";
		stbi_write_png(fileName.data(), width, height, 3, ptr_image, 0);
	}

	RenderUI_Camera(cam);
	MaterialPickGUI();

	ImGui::End();

	ImGui::Begin("Camera");
	CameraGUI();
	ImGui::End();
}

void TestGUI::MaterialPickGUI()
{
	if (ImGui::Button("Pick"))
	{
		app->bWaitPickObj = true;
	}

	if (pMaterial == nullptr) return;

	bool bAnyChange = false;
	bAnyChange |= ImGui::SliderFloat("Metallic", &pMaterial->metallic, 0.0f, 1.0f);
	bAnyChange |= ImGui::SliderFloat("Roughness", &pMaterial->roughness, 0.0f, 1.0f);

	if (bAnyChange)
	{
		renderer->ClearImage();
	}
}

void TestGUI::CameraGUI()
{
	auto pRenderData = renderer->GetRenderData();
	if (pRenderData == nullptr) return;

	auto pCamData = &pRenderData->camData;

	bool bAnyChange = false;
	bAnyChange |= ImGui::InputFloat("Film", &pCamData->film, 0.01f, 1.0f, "%.3f");
	bAnyChange |= ImGui::InputFloat("Lens", &pCamData->lens, 0.01f, 1.0f, "%.3f");
	bAnyChange |= ImGui::InputFloat("Focal", &pCamData->focal, 0.001f, 2.0f, "%.3f");

	if (bAnyChange)
	{
		renderer->ClearImage();
	}

}

void InteractiveApp::CameraUpdate(float deltaTime)
{

	if (m_key_table[GLFW_KEY_ESCAPE] == true) return;

	if (m_controlLock) return;

	float posX = 0.0f;
	float posY = 0.0f;
	float posZ = 0.0f;
	const float MoveSpeed = 20.0f;

	if (m_key_table[GLFW_KEY_W] == true)
	{
		posZ += MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}
	else if (m_key_table[GLFW_KEY_S] == true)
	{
		posZ -= MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}
	else if (m_key_table[GLFW_KEY_A] == true)
	{
		posX -= MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}
	else if (m_key_table[GLFW_KEY_D] == true)
	{
		posX += MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}
	else if (m_key_table[GLFW_KEY_Q] == true)
	{
		posY += MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}
	else if (m_key_table[GLFW_KEY_E] == true)
	{
		posY -= MoveSpeed * deltaTime;
		m_renderer->ClearImage();
	}

	if (posX != 0.0f || posY != 0.0f || posZ != 0.0f)
	{
		glm::vec3 movement = glm::vec3(posX, posY, posZ);
		m_cam->pos += m_cam->rotation * movement;
	}


	if (m_mouse_table[GLFW_MOUSE_BUTTON_1] == true)
	{

		if (bPickObj)
		{
			bPickObj = false;

			float mouseX, mouseY;
			m_appWindow->GetMousePos(mouseX, mouseY);
			Material* pMat = nullptr;
			bool hitAny = ((PathTraceRenderer*)m_renderer)->IntersectTest(mouseX, mouseY, pMat);
			if (hitAny)
			{
				m_testGUI.pMaterial = (PBMaterial*)pMat;
			}
		}


		if (!m_prevMouseClicked)
		{
			float mouseX, mouseY;
			m_appWindow->GetMousePos(mouseX, mouseY);
			m_mousePosX = mouseX;
			m_mousePosY = mouseY;
		}

		if (m_prevMouseClicked && ImGui::IsMouseDragging(0))
		{
			float mouseX, mouseY;
			m_appWindow->GetMousePos(mouseX, mouseY);

			float deltaX = mouseX - m_mousePosX;
			float deltaY = mouseY - m_mousePosY;

        	if( abs(deltaX) > 5 || abs(deltaY) > 5 )
        	{

				const float scale = 0.1f;
				m_rotX += glm::radians(deltaY * scale);
				m_rotY += glm::radians(deltaX * scale);

				auto q = glm::quat(glm::vec3{ m_rotX, -m_rotY, glm::radians(180.0f)});
				auto adjuestRot = glm::quatLookAt(q * glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        		m_cam->rotation = glm::normalize(q);

				/*
				auto rightAxis = m_cam->rotation * glm::vec3(1.0f, 0.0f, 0.0f);
				auto quatY = glm::angleAxis(rotY, rightAxis);

				auto upAxis = m_cam->rotation * glm::vec3(0.0f, 1.0f, 0.0f);
				auto quatX = glm::angleAxis(rotX, upAxis);


				//m_cam->rotation *= glm::normalize(glm::quat(glm::vec3(-rotY, -rotX, 0.0f)));
				//m_cam->rotation *= quatY;
				//m_cam->rotation *= quatX;
				//m_cam->rotation = glm::normalize(m_cam->rotation);

				auto rotationMatrix = glm::toMat3(m_cam->rotation);
				rotationMatrix = rotationMatrix * glm::toMat3(glm::quat(glm::vec3(-rotY, -rotX, 0.0f)));
				m_cam->rotation = glm::toQuat(rotationMatrix);
				*/
				/*
				auto eulerRot = glm::eulerAngles(m_cam->rotation);
				eulerRot.x += rotX;
				eulerRot.y += rotY;
				*/

				m_mousePosX = mouseX;
				m_mousePosY = mouseY;
				m_renderer->ClearImage();
			}
		}
	}
	m_prevMouseClicked = m_mouse_table[GLFW_MOUSE_BUTTON_1];

}



void InteractiveApp::Run()
{
	float time = (float)glfwGetTime();
	m_timeStart = time;	
	m_running = true;
	m_renderer->StartRender();
    float myColor[4];
	while(m_running && !m_appWindow->WindowShouldClose())
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - time;
		time = currentTime;
		m_timePass = currentTime - m_timeStart;
        
        // bool bShow = true;
        // ImGui::ShowDemoWindow(&bShow);

        CameraUpdate(deltaTime);

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
