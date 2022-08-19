#include <functional>
#include "DirectRenderApp.h"
#include "AppWindowGLFW.h"
#include "Renderer/VulkanRenderer.h"
#include "Renderer/ParallelRenderer.h"
#include "Renderer/TestScene1.h"
#include "Renderer/PathTraceRenderer.h"
#include <fstream>       //¸ü¤Jfstream¼ÐÀYÀÉ
#include <ctime>
#include <sstream>
#include <chrono>
#include <thread>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Common/stb_image_write.h"
#endif // !STB_IMAGE_WRITE_IMPLEMENTATION

DirectRenderApp::DirectRenderApp(char* argv[]) : m_running(false)
{
	const int kWidth = 1920 / 1;// 1920 / 1;//720;
	const int kHeight = 1080 / 1;// 1080 / 1;//512;

	m_renderer = new PathTraceRenderer();
	m_renderer->SetLimitIteration(true);
	m_renderer->SetMaxLimitIteration(16);
	m_scene = make_test_scene1();

	//m_scene->BuildTree();
	m_cam = new Camera(kWidth, kHeight, Vec3(200.0f, 0.0f, -200.0f));
	m_cam->rotation = glm::quatLookAt(glm::vec3(0.0f, 0.4f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_cam->pos = glm::dvec3(0, 0, 10);

	m_renderer->SetRenderData(m_scene, m_cam);
}

DirectRenderApp::~DirectRenderApp()
{
	delete m_renderer;
	delete m_scene;
	delete m_cam;
}


void DirectRenderApp::SignalCloseApp()
{
	m_running = false;
}


void DirectRenderApp::Run()
{
	float time = (float)glfwGetTime();
	m_timeStart = time;	
	m_running = true;
	m_renderer->StartRender();

    std::cout << "Start Render" << std::endl;

	std::chrono::milliseconds timespan(500);
	while (m_renderer->IsRendering())
	{
		std::this_thread::sleep_for(timespan);
	}

    std::cout << "Render Finished." << std::endl;	

	unsigned char *  ptr_image = (unsigned char*)m_renderer->GetImage();
	auto height = m_renderer->GetHeight();
	auto width = m_renderer->GetWidth();
	stbi_flip_vertically_on_write(1);
	stbi_write_png("DirectResult.png", width, height, 3, ptr_image, 0);
}
