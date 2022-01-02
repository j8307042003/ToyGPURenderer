#include "PathTraceRenderer.h"



void PathTraceRenderer::StartRender()
{
	const auto processor_count = std::thread::hardware_concurrency();
	for (int i = 0; i < processor_count.size(); ++i)
	{
		m_thread.push_back();
	}

}

void PathTraceRenderer::UpdateFrame()
{
	
}

void PathTraceRenderer::ClearImage()
{
	
}