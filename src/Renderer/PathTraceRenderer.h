#pragma once 
#include "Renderer.h"
#include <thread>
#include <vector>
#include "Core/JobSys/JobScheduler.h"
#include "Core/JobSys/Job.h"

class PathTraceRenderer : public Renderer
{
public:
	virtual void StartRender();
	virtual void UpdateFrame();
	virtual void ClearImage();
	virtual void* GetImage() override;


private:
	char* m_imageBuffer;
	float* m_integrater;
	int* sampleCount;
	void RenderLoop();
	void TestRender(int x, int y, int width, int height);


public:
	int Iteration() { return iteration; }

private:
	int iteration;
	// task lists
	// threads
	std::vector<std::thread> m_threads;

	std::thread m_renderThread;

	//
	SceneData m_sceneData;

	// Scheduler
	JobScheduler m_JobScheduler;

};