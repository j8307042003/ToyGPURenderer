#include "PathTraceRenderer.h"
#include <functional>
#include <Renderer/Camera/Camera.h>
#include <Renderer/PathTrace/PathTraceRdrMethod.h>
#include <thread>
#include <chrono>
void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
	m_imageBuffer = new char[kWidth * kHeight * 3];

	MakeSceneData(*s, m_sceneData);
	m_renderThread = std::thread(&PathTraceRenderer::RenderLoop, this);
}

void* PathTraceRenderer::GetImage()
{
	return m_imageBuffer;
}

void PathTraceRenderer::RenderLoop()
{
	const int kWidth = 512;
	const int kHeight = 512;

	m_JobScheduler.setWorkerCount(JobScheduler::getMaxWorkerCount());

	const int kTilePixels = 32;
	const int widthCellNum = kWidth / kTilePixels;
	const int heightCellNum = kHeight / kTilePixels;

	iteration = 0;

	while (true)
	{
		for (int i = 0; i < widthCellNum; ++i)
		{
			for (int j = 0; j < heightCellNum; ++j)
			{
				int x = kTilePixels * i;
				int y = kTilePixels * j;
				int w = std::min(kTilePixels, kWidth - (kTilePixels * (i + 1)));
				int h = std::min(kTilePixels, kHeight - (kTilePixels * (i + 1)));

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

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * 512) * 3;

			auto ray = SampleCamRay(camData, camPos, glm::vec3(.0f, .0f, 1.0f), glm::vec2(512, 512), glm::vec2(nowX, nowY));

			auto result = renderMethod.Sample(renderData, nowX, nowY, glm::vec2(512, 512));

			glm::vec3 color = (ray.direction * 0.5f + 0.5f) * 255.0f;
			color = result * 255.0f;

			m_imageBuffer[currentPixPos] = 128;
			m_imageBuffer[currentPixPos+1] = ((i == 0) | (i == (width-1)) | (j == 0) | (j == (height - 1))) ? 0 : 128;
			m_imageBuffer[currentPixPos+2] = 0;

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