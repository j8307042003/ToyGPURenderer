#include "PathTraceRenderer.h"
#include <functional>


void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
	m_imageBuffer = new char[kWidth * kHeight * 3];

	const auto processor_count = std::thread::hardware_concurrency();

	int widthPerCell = kWidth / (float)processor_count;
	int heightPerCell = kHeight / (float)processor_count;

	for (int i = 0; i < processor_count; ++i)
	{
		int x = widthPerCell * i;
		int y = heightPerCell * i;
		int w = std::min(widthPerCell, kWidth - (widthPerCell * (i+1)));
		int h = std::min(heightPerCell, kHeight - (heightPerCell * (i + 1)));
		auto f = std::bind(&PathTraceRenderer::TestRender, this, std::placeholders::_1,
							x, std::placeholders::_2,
							y, std::placeholders::_3,
							w, std::placeholders::_4,
							h, std::placeholders::_5);

		m_threads.push_back(std::thread(&PathTraceRenderer::TestRender, this, x, y, w, h));
	}

}

void* PathTraceRenderer::GetImage()
{
	return m_imageBuffer;
}

void PathTraceRenderer::TestRender(int x, int y, int width, int height)
{
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * 512) * 3;

			m_imageBuffer[currentPixPos] = 128;
			m_imageBuffer[currentPixPos+1] = 128;
			m_imageBuffer[currentPixPos+2] = 0;
		}
}


void PathTraceRenderer::UpdateFrame()
{
	
}

void PathTraceRenderer::ClearImage()
{
	
}