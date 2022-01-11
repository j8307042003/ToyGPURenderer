#include "PathTraceRenderer.h"
#include <functional>
#include <Renderer/Camera/Camera.h>
#include <Renderer/PathTrace/PathTraceRdrMethod.h>
#include <thread>
#include <chrono>
#include <math.h> 

void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
    int width = cam->GetWidth();
    int height = cam->GetHeight();
	m_imageBuffer = new char[width * height * 3];

	MakeSceneData(*s, m_sceneData);
	m_renderThread = std::thread(&PathTraceRenderer::RenderLoop, this);
}

void* PathTraceRenderer::GetImage()
{
	return m_imageBuffer;
}

void PathTraceRenderer::RenderLoop()
{
	//const int kWidth = 512;
	//const int kHeight = 512;
    
    int width = cam->GetWidth();
    int height = cam->GetHeight();

	m_JobScheduler.setWorkerCount(JobScheduler::getMaxWorkerCount());

	const int kTilePixels = 32;
	const int widthCellNum = ceil(width / (float)kTilePixels);
	const int heightCellNum = ceil(height / (float)kTilePixels);

	iteration = 0;

	while (true)
	{
		for (int i = 0; i < widthCellNum; ++i)
		{
			for (int j = 0; j < heightCellNum; ++j)
			{
				int x = kTilePixels * i;
				int y = kTilePixels * j;
				int w = std::min(kTilePixels, width - (kTilePixels * i));
				int h = std::min(kTilePixels, height - (kTilePixels * j));

				std::function<void()> f = [=]() {TestRender(x, y, w, h); };

				m_JobScheduler.schedule(f);
			}
		}

		while (!m_JobScheduler.empty()) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

		iteration++;
	}
}



void PathTraceRenderer::TestRender(int x, int y, int width, int height)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	auto camData = DefaultCameraData();
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);

	RenderData renderData = {};
	renderData.camData = camData;
	renderData.camDirection = glm::vec3(0, 0, 1);
	renderData.camPosition = glm::vec3();
	renderData.scene = s;
	renderData.sceneData = &m_sceneData;

	PathTraceRdrMethod renderMethod = {};

    int filmWidth = cam->GetWidth();
    int filmHeight = cam->GetHeight();
    
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;

			//auto ray = SampleCamRay(camData, camPos, glm::vec3(.0f, .0f, 1.0f), glm::vec2(filmWidth, filmHeight), glm::vec2(nowX, nowY));

			auto result = renderMethod.Sample(renderData, nowX, nowY, glm::vec2(filmWidth, filmHeight));

			//glm::vec3 color = (ray.direction * 0.5f + 0.5f) * 255.0f;
            glm::vec3 color = result * 255.0f;

			//m_imageBuffer[currentPixPos] = 128;
			//m_imageBuffer[currentPixPos+1] = ((i == 0) | (i == (width-1)) | (j == 0) | (j == (height - 1))) ? 0 : 128;
			//m_imageBuffer[currentPixPos+2] = 0;

			m_imageBuffer[currentPixPos] = (int)color.x;
			m_imageBuffer[currentPixPos + 1] = (int)color.y;
			m_imageBuffer[currentPixPos + 2] = (int)color.z;
		}
}


void PathTraceRenderer::UpdateFrame()
{

	
}

void PathTraceRenderer::ClearImage()
{
	
}
