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
#include "Texture/Texture.h"
#include <glm/geometric.hpp>




void PathTraceRenderer::StartRender()
{
	const int kWidth = 512;
	const int kHeight = 512;
    int width = cam->GetWidth();
    int height = cam->GetHeight();
	int whSize = width * height;
	long long rgbBufferSize = (long long)width * height * 3;
	m_imageBuffer = new unsigned char[rgbBufferSize]();
	m_integrater = new float[rgbBufferSize]();
	sampleCount = new int[whSize]();


	// setup denoiser buffers
	m_oidnDevice = oidn::newDevice();
	m_oidnDevice.commit();
	m_colorBuffer = new float[rgbBufferSize];
	m_albedoBuffer = new float[rgbBufferSize];
	m_normalBuffer = new float[rgbBufferSize];
	m_simpleShadingBuffer = new float[rgbBufferSize];
	m_denoiseOutputBuffer = new float[rgbBufferSize];

	m_filterRef = m_oidnDevice.newFilter("RT"); // generic ray tracing filter
	m_filterRef.setImage("color",  m_colorBuffer,  oidn::Format::Float3, width, height); // beauty
	m_filterRef.setImage("albedo", m_albedoBuffer, oidn::Format::Float3, width, height); // auxiliary
	m_filterRef.setImage("normal", m_normalBuffer, oidn::Format::Float3, width, height); // auxiliary
	m_filterRef.setImage("output", m_denoiseOutputBuffer, oidn::Format::Float3, width, height); // denoised beauty
	m_filterRef.set("cleanAux", true); // auxiliary images will be prefiltered
	m_filterRef.commit();

	MakeSceneData(*s, m_sceneData, true);
	//bvh_buildTree1(&m_sceneData, m_bvh);


	auto camData = DefaultCameraData();
	m_renderData.camData = camData;
	//m_renderData.camDirection = glm::dvec3(0, 0, -1);
	//m_renderData.camPosition = glm::dvec3(0, 0, 10);
	m_renderData.camDirection = cam->rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	m_renderData.camPosition = cam->pos;	
	m_renderData.scene = s;
	m_renderData.sceneData = &m_sceneData;	

	m_rendering = true;
	m_renderThread = std::thread(&PathTraceRenderer::RenderLoop, this);
}

void* PathTraceRenderer::GetImage()
{
	return m_imageBuffer;
}

void PathTraceRenderer::RenderLoop()
{
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
			instance->Trace(data.x, data.y, data.z, data.w);
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

	class ApplySampleChannelJob
	{
	private:
		std::vector<glm::ivec4>* tileDatas;
		PathTraceRenderer* instance;
	public:
		bool bAlbedo = false;
		bool bDonoised = false;
		bool bNormal = false;
		bool bSimpleShading = false;

	public:
		ApplySampleChannelJob(std::vector<glm::ivec4>* tileJobs, PathTraceRenderer* instance) : tileDatas(tileJobs), instance(instance) {}

		void operator()(int index) const
		{
			auto data = (*tileDatas)[index];

			if (bAlbedo)
			{
				instance->ApplyAlbedoChannelImage(data.x, data.y, data.z, data.w);
			}
			else if (bNormal)
			{
				instance->ApplyNormalChannelImage(data.x, data.y, data.z, data.w);
			}
			else if (bSimpleShading)
			{
				instance->ApplySimpleShadingImage(data.x, data.y, data.z, data.w);
			}
			else if (bDonoised)
			{
				instance->ApplyDenoiser(data.x, data.y, data.z, data.w);
			}
			else
			{
				instance->ApplyRawImage(data.x, data.y, data.z, data.w);
			}
		}
	};	

	int width = cam->GetWidth();
	int height = cam->GetHeight();

	const int kTilePixels = 64;
	const int widthCellNum = ceil(width / (float)kTilePixels);
	const int heightCellNum = ceil(height / (float)kTilePixels);
	const int tileNum = widthCellNum * heightCellNum;

	// Set Tile Data
	std::vector<glm::ivec4> tileRenderDatas = std::vector<glm::ivec4>(tileNum);
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
	TileSampleChannelJob sampleChannelWork = TileSampleChannelJob(&tileRenderDatas, this);
	ApplySampleChannelJob applyChannelWork = ApplySampleChannelJob(&tileRenderDatas, this);

    std::cout << "TBB crash stage 1" << std::endl;
	// Sample Albedo, Normal Buffer for denoiser
	tbb::parallel_for(size_t(0), tileRenderDatas.size(), sampleChannelWork);

	iteration = 0;
	while (!bLimitIteration || (bLimitIteration && maxIteration > iteration))
	{

		if (m_resetFlag)
		{
			m_resetFlag = false;
            std::cout << "TBB crash stage 2" << std::endl;
			tbb::parallel_for(size_t(0), tileRenderDatas.size(), sampleChannelWork);
		}

		bool bIterate = false;
		bool bDenoised = false;
		bool bAlbedo = false;
		bool bNormal = false;
		bool bSimpleShading = false;

		switch(m_displayChannel)
		{
			case DisplayChannel::RawImage: bIterate = true; break;
			case DisplayChannel::Denoised: bIterate = true; bDenoised = true; break;
			case DisplayChannel::Albedo: bAlbedo = true; break;
			case DisplayChannel::Normal: bNormal = true; break;
			case DisplayChannel::SimpleShading: bSimpleShading = true; break;
			default: break;
		}

		if (bIterate)
		{
            std::cout << "TBB crash stage 3" << std::endl;
			tbb::parallel_for(size_t(0), tileRenderDatas.size(), renderWork);
			iteration++;
		}

		if (bDenoised)
		{
			m_filterRef.execute();

			// Check for errors
			const char* errorMessage;
			if (m_oidnDevice.getError(errorMessage) != oidn::Error::None)
				std::cout << "Error: " << errorMessage << std::endl;
		}

		applyChannelWork.bAlbedo = bAlbedo;
		applyChannelWork.bDonoised = bDenoised;
		applyChannelWork.bNormal = bNormal;
		applyChannelWork.bSimpleShading = bSimpleShading;
        std::cout << "TBB crash stage 4" << std::endl;
		tbb::parallel_for(size_t(0), tileRenderDatas.size(), applyChannelWork);
		//std::cout << "Complete Iteration : " << iteration << std::endl;
	}

	m_rendering = false;
}

void PathTraceRenderer::ApplyDenoiser(int x, int y, int width, int height)
{
	int filmWidth = cam->GetWidth();
	int filmHeight = cam->GetHeight();
	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			m_imageBuffer[currentPixPos] = (int)(glm::pow(m_denoiseOutputBuffer[currentPixPos], 1.0f / 2.2f) * 255.0f);
			m_imageBuffer[currentPixPos + 1] = (int)(glm::pow(m_denoiseOutputBuffer[currentPixPos + 1], 1.0f / 2.2f) * 255.0f);
			m_imageBuffer[currentPixPos + 2] = (int)(glm::pow(m_denoiseOutputBuffer[currentPixPos + 2], 1.0f / 2.2f) * 255.0f);
		}
}

void PathTraceRenderer::ApplyAlbedoChannelImage(int x, int y, int width, int height)
{
	int filmWidth = cam->GetWidth();
	int filmHeight = cam->GetHeight();
	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			m_imageBuffer[currentPixPos] = (int)(glm::pow(m_albedoBuffer[currentPixPos], 1.0f / 2.2f) * 255.0f);
			m_imageBuffer[currentPixPos + 1] = (int)(glm::pow(m_albedoBuffer[currentPixPos + 1], 1.0f / 2.2f) * 255.0f);
			m_imageBuffer[currentPixPos + 2] = (int)(glm::pow(m_albedoBuffer[currentPixPos + 2], 1.0f / 2.2f) * 255.0f);
		}	
}

void PathTraceRenderer::ApplyNormalChannelImage(int x, int y, int width, int height)
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

			m_imageBuffer[currentPixPos] = (int)(m_normalBuffer[currentPixPos] * 255.0f);
			m_imageBuffer[currentPixPos + 1] = (int)(m_normalBuffer[currentPixPos + 1] * 255.0f);
			m_imageBuffer[currentPixPos + 2] = (int)(m_normalBuffer[currentPixPos + 2] * 255.0f);
		}	
}

void PathTraceRenderer::ApplySimpleShadingImage(int x, int y, int width, int height)
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

			m_imageBuffer[currentPixPos] = (int)(m_simpleShadingBuffer[currentPixPos] * 255.0f);
			m_imageBuffer[currentPixPos + 1] = (int)(m_simpleShadingBuffer[currentPixPos + 1] * 255.0f);
			m_imageBuffer[currentPixPos + 2] = (int)(m_simpleShadingBuffer[currentPixPos + 2] * 255.0f);
		}	
}


void PathTraceRenderer::ApplyRawImage(int x, int y, int width, int height)
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

			m_imageBuffer[currentPixPos] = (uint8_t)(std::max(0.0f, std::min(255.0f, glm::pow(m_colorBuffer[currentPixPos], 1.0f / 2.2f) * 255.0f)));
			m_imageBuffer[currentPixPos + 1] = (uint8_t)(std::max(0.0f, std::min(255.0f, glm::pow(m_colorBuffer[currentPixPos + 1], 1.0f / 2.2f) * 255.0f)));
			m_imageBuffer[currentPixPos + 2] = (uint8_t)(std::max(0.0f, std::min(255.0f, glm::pow(m_colorBuffer[currentPixPos + 2], 1.0f / 2.2f) * 255.0f)));
		}
}


void PathTraceRenderer::SampleDenoiserBaseImage(int x, int y, int width, int height)
{
	/*
	auto camData = DefaultCameraData();
	RenderData m_renderData = {};
	m_renderData.camData = camData;
	//m_renderData.camDirection = glm::dvec3(0, 0, -1);
	//m_renderData.camPosition = glm::dvec3(0, 0, 10);
	m_renderData.scene = s;
	m_renderData.sceneData = &m_sceneData;
	*/
	m_renderData.camPosition = cam->pos;	
	m_renderData.camDirection = cam->rotation * glm::vec3(0.0f, 0.0f, 1.0f);

	const int BVH_Stack_Num = 128;
	int bvh_stack[BVH_Stack_Num];

    int filmWidth = cam->GetWidth();
    int filmHeight = cam->GetHeight();
    glm::vec2 uv;
	glm::vec3 lightDir = glm::normalize(glm::vec3(0.3f, 0.8f, 0.0f));
	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			auto filmRes = glm::vec2(filmWidth, filmHeight);
			const auto cam_ray = SampleCamRay(m_renderData.camData, m_renderData.camPosition, m_renderData.camDirection, filmRes, glm::vec2(nowX, nowY));
			Ray3f ray = cam_ray;
			HitInfo hitInfo;
			glm::dvec3 rayHitPosition = {};
			glm::dvec3 rayHitNormal = {};
			uv = {};
			int shapeIndex = -1;
			//bool bHitAny = RayTrace(*m_renderData.sceneData, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, shapeIndex);
			//bool bHitAny = BHV_Raycast(m_renderData.sceneData, m_bvh, ray, 0.1f, 10000.0f, rayHitPosition, rayHitNormal, uv, shapeIndex, BVH_Stack_Num / 2, &bvh_stack[0]);
			SceneIntersectData intersect;
			bool bHitAny = IntersectScene(m_renderData.sceneData, ray, 0.1f, 10000.0f, intersect);
			rayHitPosition = intersect.point;
			rayHitNormal = intersect.normal;
			shapeIndex = intersect.shapeIdx;
			uv = intersect.uv;

			glm::vec3 albedo = {};
			if (bHitAny)
			{
				auto mat = GetShapeMaterial(*m_renderData.sceneData, shapeIndex);
				SurfaceData surface = {};
				surface.position = rayHitPosition;
				surface.normal = rayHitNormal;
				surface.uv = uv;
				albedo = mat->Albedo(surface);

				/*
				auto shapePtr = GetShapeData(*m_renderData.sceneData, shapeIndex);
				if (shapePtr != nullptr && shapePtr.type == ShapeType::Triangle)
				{
					auto primitiveIdx = shapePtr->primitiveId;	
					auto triangleData = sceneData.shapesData.triangles[primitiveIdx];
					m_renderData.sceneData.shapesData.positions[triangleData.x]
				}
				*/
			}

			m_albedoBuffer[currentPixPos] = albedo.x;
			m_albedoBuffer[currentPixPos + 1] = albedo.y;
			m_albedoBuffer[currentPixPos + 2] = albedo.z;

			m_normalBuffer[currentPixPos] = (float) rayHitNormal.x * 0.5f + 0.5f;
			m_normalBuffer[currentPixPos + 1] = (float) rayHitNormal.y * 0.5f + 0.5f;
			m_normalBuffer[currentPixPos + 2] = (float) rayHitNormal.z * 0.5f + 0.5f;			

			float lighting = glm::dot(glm::vec3(rayHitNormal), lightDir) * 0.5f + 0.5f;
			auto diffuse = glm::vec3(albedo) * std::min(1.0f, lighting + 0.1f);
			m_simpleShadingBuffer[currentPixPos] = diffuse.x;
			m_simpleShadingBuffer[currentPixPos + 1] = diffuse.y;
			m_simpleShadingBuffer[currentPixPos + 2] = diffuse.z;

			//m_normalBuffer[currentPixPos] = uv.x;//(float) rayHitNormal.x;
			//m_normalBuffer[currentPixPos + 1] = uv.y;//(float) rayHitNormal.y;
			//m_normalBuffer[currentPixPos + 2] = uv.x < 0.0f || uv.y < 0.0f ? 1.0f : 0.0f;//(float) rayHitNormal.z;
		}
}


void PathTraceRenderer::Trace(int x, int y, int width, int height)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	auto camData = DefaultCameraData();
	glm::dvec3 camPos = glm::dvec3(0.0f, 0.0f, 0.0f);

	/*
	RenderData m_renderData = {};
	m_renderData.camData = camData;
	//m_renderData.camDirection = glm::dvec3(0, 0, -1);
	//m_renderData.camPosition = glm::dvec3(0, 0, 10);
	m_renderData.scene = s;
	m_renderData.sceneData = &m_sceneData;
	*/
	m_renderData.camDirection = cam->rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	m_renderData.camPosition = cam->pos;

	PathTraceRdrMethod renderMethod = {};
	renderMethod.bvh_tree = &m_bvh;

    int filmWidth = cam->GetWidth();
    int filmHeight = cam->GetHeight();
    
	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i)
		{
			int nowX = x + i;
			int nowY = y + j;

			int currentPixPos = (nowX + nowY * filmWidth) * 3;
			int sampleCountIndex = nowX + nowY * filmWidth;

			auto result = renderMethod.Sample(m_renderData, nowX, nowY, glm::vec2(filmWidth + (SysRandom::Random() - 0.5f) * 2.0f, filmHeight + (SysRandom::Random() - 0.5f) * 2.0f));
			result = glm::clamp(result, glm::vec3(0.0f), glm::vec3(0.99f));

			m_integrater[currentPixPos] += result.x;
			m_integrater[currentPixPos + 1] += result.y;
			m_integrater[currentPixPos + 2] += result.z;
			sampleCount[sampleCountIndex]++;

			glm::vec3 color_float = glm::vec3(m_integrater[currentPixPos], m_integrater[currentPixPos + 1], m_integrater[currentPixPos + 2]) / (float)sampleCount[sampleCountIndex];
			glm::vec3 color = glm::clamp(color_float * 255.0f, glm::vec3(0.0f), glm::vec3(255.0f));
			
			m_colorBuffer[currentPixPos] = color_float.x;
			m_colorBuffer[currentPixPos + 1] = color_float.y;
			m_colorBuffer[currentPixPos + 2] = color_float.z;
		}
}


void PathTraceRenderer::UpdateFrame()
{

	
}

void PathTraceRenderer::ClearImage()
{
	int width = cam->GetWidth();
	int height = cam->GetHeight();
	int whRes = width * height;
	int rgbBufferSize = whRes * 3;
	memset(m_imageBuffer, 0, rgbBufferSize);
	memset(m_integrater, 0, rgbBufferSize * sizeof(float));
	memset(sampleCount, 0, whRes * sizeof(int));
	iteration = 0;
	m_resetFlag = true;
}

bool PathTraceRenderer::IntersectTest(int posX, int posY, Material* & pMaterial)
{
	auto camData = DefaultCameraData();
	int filmWidth = cam->GetWidth();
	int filmHeight = cam->GetHeight();
	auto filmRes = glm::vec2(filmWidth, filmHeight);
	auto camDirection = cam->rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	const auto cam_ray = SampleCamRay(camData, cam->pos, camDirection, filmRes, glm::vec2(posX, posY));
	SceneIntersectData intersect;
	if (!IntersectScene(&m_sceneData, cam_ray, 0.1f, 10000.0f, intersect)) return false;

	pMaterial = GetMaterial(m_sceneData, intersect.materialIdx);
	return true;
}
