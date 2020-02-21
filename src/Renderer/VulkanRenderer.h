#pragma once
#include "Renderer.h"
#include "VulkanLib/vkHelper.h"
#include "VulkanLib/VulkanInstance.h"


#define VkRenderer_SHAPE_TYPE_SPHERE 1
#define VkRenderer_SHAPE_TYPE_TRIANGLE 2
#define VkRenderer_SHAPE_TYPE_PLANE 3


Vec3 make_normal(const Vec3 & v1, const Vec3 & v2, const Vec3 & v3);


	struct vkRenderData
	{
	  uint width;
	  uint height; 
	  int time;
	  uint sampleCount;
	};

	#pragma pack(push, 1)
	struct SphereShape {
	  Vec3 pos;
	  float pad;
	  float radius;
	  float pad_1, pad_2, pad_3; 
	};
	#pragma pack(pop)


	#pragma pack(push, 1)
	struct TriangleShape {
	  Vec3 v1;
	  float p0;
	  Vec3 v2;
	  float p1;
	  Vec3 v3;
	  float p2;
	  Vec3 normal;
	  float p3;

	  TriangleShape(Vec3 v1, Vec3 v2, Vec3 v3) : 
	    v1(v1), v2(v2), v3(v3),
	    normal(make_normal(v1, v2, v3)) {}
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct PlaneShape {
	  TriangleShape t1;
	  TriangleShape t2;

	  PlaneShape( Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) : 
	  t1(v0, v1, v2),
	  t2(v1, v3, v2)
	  {}
	};
	#pragma pack(pop)


	struct GPU_Material {
		float r;
		float g;
		float b;
		float a;

		float emission_r;
		float emission_g;
		float emission_b;
		float roughness;
	};


  	struct ShapeInstance {
  		int shapeType;
  		int shapeIdx;
  		int materialIdx;
  	};  



class VulkanRenderer : public Renderer {
	private:
		VulkanLib::VulkanInstance vulkanInstance;
		VulkanLib::PipelineObj pipeline;

		std::vector<BufferObject> buffers;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		VkQueue queue;

		vkRenderData render_data;

		std::vector<ShaderAttri> renderBufferAttribute;
		std::thread renderThread;
		int iteratorCount;
		void * mem;

	public:
		VulkanRenderer();
		~VulkanRenderer();
		void StartRender();
		void UpdateFrame();

	private:
		void SetData();
		void SubmitRender();
		void ClenaBuffer();
		void InitBuffer();
		void RenderTask();
};