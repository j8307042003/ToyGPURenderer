#include "PathTraceRenderer.h"
#include <functional>
#include <Renderer/Camera/Camera.h>
#include <Renderer/PathTrace/PathTraceRdrMethod.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "Random/SysRandom.h"
#include <Renderer/RayTrace/RayTrace.h>
#include <oneapi/tbb/parallel_for.h>


void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
    int width = cam->GetWidth();
    int height = cam->GetHeight();
	m_imageBuffer = new char[width * height * 3]();
	m_integrater = new float[width * height * 3]();
	sampleCount = new int[width * height]();	


	// setup denoiser buffers
	m_oidnDevice = oidn::newDevice();
	m_oidnDevice.commit();
	m_colorBuffer = new float[width * height * 3];
	m_albedoBuffer = new float[width * height * 3];
	m_normalBuffer = new float[width * height * 3];
	m_denoiseOutputBuffer = new float[width * height * 3];

	m_filterRef = m_oidnDevice.newFilter("RT"); // generic ray tracing filter
	m_filterRef.setImage("color",  m_colorBuffer,  oidn::Format::Float3, width, height); // beauty
	m_filterRef.setImage("albedo", m_albedoBuffer, oidn::Format::Float3, width, height); // auxiliary
	m_filterRef.setImage("normal", m_normalBuffer, oidn::Format::Float3, width, height); // auxiliary
	m_filterRef.setImage("output", m_denoiseOutputBuffer, oidn::Format::Float3, width, height); // denoised beauty
	m_filterRef.set("cleanAux", true); // auxiliary images will be prefiltered
	m_filterRef.commit();

	MakeSceneData(*s, m_sceneData);
	bvh_buildTree1(&m_sceneData, m_bvh);
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

	//m_JobScheduler.setWorkerCount(JobScheduler::getMaxWorkerCount());

	const int kTilePixels = 64;
	const int widthCellNum = ceil(width / (float)kTilePixels);
	const int heightCellNum = ceil(height / (float)kTilePixels);

	class TileRenderJob
	{
	private:
		std::vector<glm::ivec4> * tileDatas;
		PathTraceRenderer* instance;
	public:
		TileRenderJob(std::vector<glm::ivec4> * tileJobs, PathTraceRenderer * instance) : tileDatas(tileJobs), instance(instance){}

		void operator()(int index) const
		{
			auto data = (*tileDatas)[index];
			instance->TestRender(data.x, data.y, data.z, data.w);
		}
	};

	class TileApplyDenoiseJob
	{
	private:
		std::vector<glm::ivec4>* tileDatas;
		PathTraceRenderer* instance;
	public:
		TileApplyDenoiseJob(std::vector<glm::ivec4>* tileJobs, PathTraceRenderer* instance) : tileDatas(tileJobs), instance(instance) {}

		void operator()(int index) const
		{
			auto data = (*tileDatas)[index];
			instance->ApplyDenoiser(data.x, data.y, data.z, data.w);
		}
	};

	class TileSampleChannelJob
	{
	private:
		std::vector<glm::ivec4>* tileDatas;
		PathTraceRenderer* instance;
	public:
		TileSampleChannelJob(std::vector<glm::ivec4>* tileJobs, PathTraceRenderer* instance) : tileDatas(tileJobs), instance(instance) {}

		void operator()(int index) const
		{
			auto data = (*tileDatas)[index];
			instance->SampleDenoiserBaseImage(data.x, data.y, data.z, data.w);
		}
	};

	// Set Tile Data
	std::vector<glm::ivec4> tileRenderDatas = std::vector<glm::ivec4>(widthCellNum * heightCellNum);
	for (int i = 0; i < widthCellNum; ++i)
	{
		for (int j = 0; j < heightCellNum; ++j)
		{
			int x = kTilePixels * i;
			int y = kTilePixels * j;
			int w = std::min(kTilePixels, width - (kTilePixels * i));
			int h = std::min(kTilePixels, height - (kTilePixels * j));

			tileRenderDatas.push_back({ x, y, w, h });
		}
	}


	// Make Job
	TileRenderJob renderWork = TileRenderJob(&tileRenderDatas, this);
	TileApplyDenoiseJob applyDenoiseWork = TileApplyDenoiseJob(&tileRenderDatas, this);
	TileSampleChannelJob sampleChannelWork = TileSampleChannelJob(&tileRenderDatas, this);

	tbb::parallel_for(size_t(0), tileRenderDatas.size(), sampleChannelWork);


	iteration = 0;
	while (true)
	{
		tbb::parallel_for(size_t(0), tileRenderDatas.size(), renderWork);
		iteration++;

		if (m_showDenoiser)
		{
			m_filterRef.execute();

			// Check for errors
			const char* errorMessage;
			if (m_oidnDevice.getError(errorMessage) != oidn::Error::None)
				std::cout << "Error: " << errorMessage << std::endl;

			tbb::parallel_for(size_t(0), tileRenderDatas.size(), applyDenoiseWork);
		}
	}
}

void PathTraceRenderer::ApplyDenoiser(int x, int y, int width, int height)
{
	int filmWidth = cam->GetWidth();
	int filmHeight = cam->GetHeight();
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			m_imageBuffer[currentPixPos] = (int)(m_denoiseOutputBuffer[currentPixPos] * 255.0f);
			m_imageBuffer[currentPixPos + 1] = (int)(m_denoiseOutputBuffer[currentPixPos + 1] * 255.0f);
			m_imageBuffer[currentPixPos + 2] = (int)(m_denoiseOutputBuffer[currentPixPos + 2] * 255.0f);
		}
}


void PathTraceRenderer::SampleDenoiserBaseImage(int x, int y, int width, int height)
{
	auto camData = DefaultCameraData();
	glm::dvec3 camPos = glm::dvec3(0.0f, 0.0f, 0.0f);

	RenderData renderData = {};
	renderData.camData = camData;
	renderData.camDirection = glm::dvec3(0, 0, -1);
	renderData.camPosition = glm::dvec3(0, 0, 10);
	renderData.scene = s;
	renderData.sceneData = &m_sceneData;

    int filmWidth = cam->GetWidth();
    int filmHeight = cam->GetHeight();
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			auto filmRes = glm::vec2(filmWidth, filmHeight);
			const auto cam_ray = SampleCamRay(renderData.camData, renderData.camPosition, renderData.camDirection, filmRes, glm::vec2(nowX, nowY));
			Ray3f ray = cam_ray;
			HitInfo hitInfo;
			glm::dvec3 rayHitPosition = {};
			glm::dvec3 rayHitNormal = {};
			int shapeIndex = -1;
			bool bHitAny = RayTrace(*renderData.sceneData, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, shapeIndex);
			
			glm::vec3 albedo = {};
			if (bHitAny)
			{
				auto mat = GetShapeMaterial(*renderData.sceneData, shapeIndex);
				albedo = mat->Albedo();
			}

			m_albedoBuffer[currentPixPos] = albedo.x;
			m_albedoBuffer[currentPixPos + 1] = albedo.y;
			m_albedoBuffer[currentPixPos + 2] = albedo.z;		
			
			m_normalBuffer[currentPixPos] = rayHitNormal.x;
			m_normalBuffer[currentPixPos + 1] = rayHitNormal.y;
			m_normalBuffer[currentPixPos + 2] = rayHitNormal.z;					
		}
}


void PathTraceRenderer::TestRender(int x, int y, int width, int height)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	auto camData = DefaultCameraData();
	glm::dvec3 camPos = glm::dvec3(0.0f, 0.0f, 0.0f);

	RenderData renderData = {};
	renderData.camData = camData;
	renderData.camDirection = glm::dvec3(0, 0, -1);
	renderData.camPosition = glm::dvec3(0, 0, 10);
	renderData.scene = s;
	renderData.sceneData = &m_sceneData;

	PathTraceRdrMethod renderMethod = {};
	renderMethod.bvh_tree = &m_bvh;

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
			m_integrater[currentPixPos + 1] += result.y;
			m_integrater[currentPixPos + 2] += result.z;
			sampleCount[sampleCountIndex]++;

			//glm::vec3 color = (ray.direction * 0.5f + 0.5f) * 255.0f;
			glm::vec3 color_float = glm::vec3(m_integrater[currentPixPos], m_integrater[currentPixPos + 1], m_integrater[currentPixPos + 2]) / (float)sampleCount[sampleCountIndex];
			glm::vec3 color = glm::clamp(color_float * 255.0f, glm::vec3(0.0f), glm::vec3(255.0f));

			//color = glm::vec3(m_integrater[currentPixPos], m_integrater[currentPixPos + 1], m_integrater[currentPixPos + 2]) * (255.0f / (float)sampleCount[sampleCountIndex]);
			//color = glm::vec3(result.x, result.y, result.z) * (255.0f / (float)sampleCount[sampleCountIndex]);
			//m_imageBuffer[currentPixPos] = 128;
			//m_imageBuffer[currentPixPos+1] = ((i == 0) | (i == (width-1)) | (j == 0) | (j == (height - 1))) ? 0 : 128;
			//m_imageBuffer[currentPixPos+2] = 0;

			m_colorBuffer[currentPixPos] = color_float.x;
			m_colorBuffer[currentPixPos + 1] = color_float.y;
			m_colorBuffer[currentPixPos + 2] = color_float.z;

			if (!m_showDenoiser)
			{
				m_imageBuffer[currentPixPos] = (int)color.x;
				m_imageBuffer[currentPixPos + 1] = (int)color.y;
				m_imageBuffer[currentPixPos + 2] = (int)color.z;
			}
		}
}


void PathTraceRenderer::UpdateFrame()
{

	
}

void PathTraceRenderer::ClearImage()
{
	int width = cam->GetWidth();
	int height = cam->GetHeight();
	memset(m_imageBuffer, 0, width * height * 3);
	memset(m_integrater, 0, width * height * 3);
	memset(sampleCount, 0, width * height);
	iteration = 0;
}
