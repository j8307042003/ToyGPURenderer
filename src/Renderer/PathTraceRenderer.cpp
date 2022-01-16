#include "PathTraceRenderer.h"
#include <functional>
#include <Renderer/Camera/Camera.h>
#include <Renderer/PathTrace/PathTraceRdrMethod.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "Random/SysRandom.h"

void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
    int width = cam->GetWidth();
    int height = cam->GetHeight();
	m_imageBuffer = new char[width * height * 3]();
	m_integrater = new float[width * height * 3]();
	sampleCount = new int[width * height]();	

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
	glm::dvec3 camPos = glm::dvec3(0.0f, 0.0f, 0.0f);

	RenderData renderData = {};
	renderData.camData = camData;
	renderData.camDirection = glm::dvec3(0, 0, 1);
	renderData.camPosition = glm::dvec3();
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
			int sampleCountIndex = nowX + nowY * filmWidth;

			//auto ray = SampleCamRay(camData, camPos, glm::vec3(.0f, .0f, 1.0f), glm::vec2(filmWidth, filmHeight), glm::vec2(nowX, nowY));

			auto result = renderMethod.Sample(renderData, nowX, nowY, glm::vec2(filmWidth + (SysRandom::Random() - 0.5f) * 2.0f, filmHeight + (SysRandom::Random() - 0.5f) * 2.0f));

			m_integrater[currentPixPos] += result.x;
			m_integrater[currentPixPos+1] += result.y;
			m_integrater[currentPixPos+2] += result.z;
			sampleCount[sampleCountIndex]++;

			//glm::vec3 color = (ray.direction * 0.5f + 0.5f) * 255.0f;
            glm::vec3 color = glm::clamp(glm::vec3(m_integrater[currentPixPos], m_integrater[currentPixPos + 1], m_integrater[currentPixPos + 2]) * (255.0f / (float)sampleCount[sampleCountIndex]), glm::vec3(0.0f), glm::vec3(255.0f));
            //color = glm::vec3(m_integrater[currentPixPos], m_integrater[currentPixPos + 1], m_integrater[currentPixPos + 2]) * (255.0f / (float)sampleCount[sampleCountIndex]);
            //color = glm::vec3(result.x, result.y, result.z) * (255.0f / (float)sampleCount[sampleCountIndex]);
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
