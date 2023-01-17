#include "EmbreeEngine.h"
#include "../Scene.h"
#include <glm/glm.hpp>
//#include <glm/geometric.hpp>

EmbreeEngine* EmbreeEngine::BuildEmgreeEngine(SceneData * sceneData)
{
	EmbreeEngine * instance = new EmbreeEngine();
	instance->device = rtcNewDevice(NULL);
	instance->scene = rtcNewScene(instance->device);
	//rtcSetSceneBuildQuality(instance->scene, RTC_BUILD_QUALITY_HIGH);

	//instance->geom = rtcNewGeometry(instance->device, RTC_GEOMETRY_TYPE_TRIANGLE);

	int triangleCount = sceneData->shapesData.triangles.size();

	auto triGeo = rtcNewGeometry(instance->device, RTC_GEOMETRY_TYPE_TRIANGLE);
	
	auto embree_positions = (float*)rtcSetNewGeometryBuffer(triGeo,
		RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),
		triangleCount * 3);
	auto embree_triangles = rtcSetNewGeometryBuffer(triGeo,
		RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned int),
		triangleCount);

	memcpy(embree_triangles, sceneData->shapesData.triangles.data(), triangleCount * 12);

	for (int i = 0; i < triangleCount; ++i)
	{
		auto& tri = sceneData->shapesData.triangles[i];
		embree_positions[i * 9 + 0] = (float)sceneData->shapesData.positions[tri[0]][0];
		embree_positions[i * 9 + 1] = (float)sceneData->shapesData.positions[tri[0]][1];
		embree_positions[i * 9 + 2] = (float)sceneData->shapesData.positions[tri[0]][2];

		embree_positions[i * 9 + 3] = (float)sceneData->shapesData.positions[tri[1]][0];
		embree_positions[i * 9 + 4] = (float)sceneData->shapesData.positions[tri[1]][1];
		embree_positions[i * 9 + 5] = (float)sceneData->shapesData.positions[tri[1]][2];

		embree_positions[i * 9 + 6] = (float)sceneData->shapesData.positions[tri[2]][0];
		embree_positions[i * 9 + 7] = (float)sceneData->shapesData.positions[tri[2]][1];
		embree_positions[i * 9 + 8] = (float)sceneData->shapesData.positions[tri[2]][2];
	}
	//memcpy(embree_positions, shape.positions.data(), shape.positions.size() * 12);
	rtcCommitGeometry(triGeo);
	rtcAttachGeometryByID(instance->scene, triGeo, 0);
	rtcReleaseGeometry(triGeo);

	rtcCommitScene(instance->scene);
	return instance;
}


bool EmbreeEngine::IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData * intersect)
{
	RTCRayHit rayhit;
	rayhit.ray.org_x = ray.origin.x; rayhit.ray.org_y = ray.origin.y; rayhit.ray.org_z = ray.origin.z;
	rayhit.ray.dir_x = ray.direction.x; rayhit.ray.dir_y = ray.direction.y; rayhit.ray.dir_z = ray.direction.z;
	rayhit.ray.tnear = t_min;
	rayhit.ray.tfar = t_max;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	rtcIntersect1(scene, &context, &rayhit);

	if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID) return false;

	auto& triangleData = sceneData->shapesData.triangles[rayhit.hit.primID];

	intersect->shapeIdx = rayhit.hit.primID;
	intersect->point = ray.origin + (double)rayhit.ray.tfar * ray.direction;
	intersect->materialIdx = sceneData->shapes[intersect->shapeIdx].matIdx;
	intersect->uv = (double)rayhit.hit.u * sceneData->shapesData.texcoords[triangleData.y] + (double)rayhit.hit.v * sceneData->shapesData.texcoords[triangleData.z] + (double)(1 - rayhit.hit.u - rayhit.hit.v) * sceneData->shapesData.texcoords[triangleData.x];
	intersect->normal = glm::normalize(
									(double)rayhit.hit.u * sceneData->shapesData.normals[triangleData.y] 
									+ (double)rayhit.hit.v * sceneData->shapesData.normals[triangleData.z]
									+ (double)(1 - rayhit.hit.u - rayhit.hit.v) * sceneData->shapesData.normals[triangleData.x]);
	intersect->tangent = glm::normalize(
									(double)rayhit.hit.u * sceneData->shapesData.tangents[triangleData.y]
									+ (double)rayhit.hit.v * sceneData->shapesData.tangents[triangleData.z]
									+ (double)(1 - rayhit.hit.u - rayhit.hit.v) * sceneData->shapesData.tangents[triangleData.x]);
	intersect->normal = glm::dot(intersect->normal, ray.direction) <= 0.0f ? intersect->normal : -intersect->normal;

	return true;
}

bool EmbreeEngine::Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max)
{
	RTCRay rtcray;
	rtcray.org_x = ray.origin.x; rtcray.org_y = ray.origin.y; rtcray.org_z = ray.origin.z;
	rtcray.dir_x = ray.direction.x; rtcray.dir_y = ray.direction.y; rtcray.dir_z = ray.direction.z;
	rtcray.tnear = t_min;
	rtcray.tfar = t_max;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	rtcOccluded1(scene, &context, &rtcray);

	return rtcray.tfar == -INFINITY;
}
