#pragma once 
#include "Renderer.h"
#include <thread>
#include <vector>
#include "Core/JobSys/JobScheduler.h"
#include "Core/JobSys/Job.h"
//#include "../../third-party/oidnlib/include/OpenImageDenoise/oidn.hpp"
#include <OpenImageDenoise/oidn.hpp>
//#include <OpenImageDenoise/oidn.hpp>
#include "Accelerate/BVHStruct.h"

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
	void SampleDenoiserBaseImage(int x, int y, int width, int height);
	void ApplyDenoiser(int x, int y, int width, int height);


public:
	int Iteration() { return iteration; }
	bool GetShowDenoise() { return m_showDenoiser; }
	void SetShowDenoise(bool show) { m_showDenoiser = show; }

private:
	int iteration;
	// task lists
	// threads
	std::vector<std::thread> m_threads;

	std::thread m_renderThread;

	BVHTree m_bvh;

	//
	SceneData m_sceneData;

	// Scheduler
	JobScheduler m_JobScheduler;

	//
	bool m_showDenoiser = true;

	//Open Image Denoiser
	oidn::DeviceRef m_oidnDevice;
	oidn::FilterRef m_filterRef;
	float* m_colorBuffer;
	float* m_albedoBuffer;
	float* m_normalBuffer;
	float* m_denoiseOutputBuffer;

};