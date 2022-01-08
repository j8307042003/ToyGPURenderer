#pragma once 
#include "Renderer.h"
#include <thread>
#include <vector>

class PathTraceRenderer : public Renderer
{
public:
	virtual void StartRender();
	virtual void UpdateFrame();
	virtual void ClearImage();
	virtual void* GetImage() override;


private:
	char* m_imageBuffer;
	void TestRender(int x, int y, int width, int height);

private:
	// task lists
	// threads
	std::vector<std::thread> m_threads;

};