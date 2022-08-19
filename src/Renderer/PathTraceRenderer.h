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
	enum class DisplayChannel
	{
		RawImage = 0,
		Denoised = 1,
		Albedo = 2,
		Normal = 3,
	};
	static DisplayChannel IntToDisplayChannel(int idx) 
	{
		switch (idx)
		{
			case 0: return DisplayChannel::RawImage;
			case 1: return DisplayChannel::Denoised;
			case 2: return DisplayChannel::Albedo;
			case 3: return DisplayChannel::Normal;
			default: break;
		}
	}

	static int DisplayChannelToInt(DisplayChannel displayChannel)
	{
		switch (displayChannel)
		{
			case DisplayChannel::RawImage: return 0;
			case DisplayChannel::Denoised: return 1;
			case DisplayChannel::Albedo: return 2;
			case DisplayChannel::Normal: return 3;
			default: break;
		}
	}



public:
	virtual void StartRender();
	virtual void UpdateFrame();
	virtual void ClearImage();
	virtual void* GetImage() override;


private:
	unsigned char* m_imageBuffer = nullptr;
	float* m_integrater;
	int* sampleCount;
	void RenderLoop();
	void Trace(int x, int y, int width, int height);
	void SampleDenoiserBaseImage(int x, int y, int width, int height);
	void ApplyDenoiser(int x, int y, int width, int height);
	void ApplyAlbedoChannelImage(int x, int y, int width, int height);
	void ApplyNormalChannelImage(int x, int y, int width, int height);
	void ApplyRawImage(int x, int y, int width, int height);


public:
	int Iteration() { return iteration; }
	void SetLimitIteration(bool limit) { bLimitIteration = limit; }
	void SetMaxLimitIteration(int it) { maxIteration = it; }
	bool IsRendering() { return m_rendering; }
	bool GetShowDenoise() { return m_showDenoiser; }
	void SetShowDenoise(bool show) { m_showDenoiser = show; }

	bool GetShowAlbedo() { return m_showAlbedo;}
	void SetShowAlbedo(bool show) {m_showAlbedo = show;}

	DisplayChannel GetShowDisplayChannel() { return m_displayChannel;}
	void SetShowDisplayChannel(DisplayChannel displayChannel) { m_displayChannel = displayChannel;}	

	int GetWidth() { return cam->GetWidth(); }
	int GetHeight() { return cam->GetHeight(); }

private:
	int iteration;
	bool bLimitIteration = false;
	int maxIteration = 16;
	bool m_rendering = false;

	std::thread m_renderThread;

	//
	SceneData m_sceneData;
	BVHTree m_bvh;

	//
	bool m_showDenoiser = true;
	bool m_showAlbedo = false;
	DisplayChannel m_displayChannel = DisplayChannel::RawImage;

	bool m_resetFlag = false;

	//Open Image Denoiser
	oidn::DeviceRef m_oidnDevice;
	oidn::FilterRef m_filterRef;
	float* m_colorBuffer;
	float* m_albedoBuffer;
	float* m_normalBuffer;
	float* m_denoiseOutputBuffer;

};