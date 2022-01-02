#pragma once 
#include "Renderer.h"
#include <thread>
#include <vector>

class PathTraceRenderer : public Renderer
{
public:
	virtual void StartRender() = 0;
	virtual void UpdateFrame() = 0;
	virtual void ClearImage() = 0;

private:
	// task lists
	// threads
	std::vector<std::thread> m_threads;

};