#include "RayTraceEngine.h"
#include "../Scene.h"

bool RayTraceEngine::IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData * intersect)
{
	const int kTraceStackDepth = 128;
	int TraceStackData[kTraceStackDepth];
	bool bHitAny = BHV_Raycast(
		sceneData, 
		*sceneData->bvh_tree, 
		ray, 
		t_min, 
		t_max, 
		intersect->point, 
		intersect->normal, 
		intersect->uv, 
		intersect->shapeIdx, 
		kTraceStackDepth, 
		&TraceStackData[0]
	);
	return bHitAny;
}

bool RayTraceEngine::Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max)
{
	const int kTraceStackDepth = 128;
	SceneIntersectData intersect;
	int TraceStackData[kTraceStackDepth];
	bool bHitAny = BHV_Raycast(
		sceneData,
		*sceneData->bvh_tree,
		ray,
		t_min,
		t_max,
		intersect.point,
		intersect.normal,
		intersect.uv,
		intersect.shapeIdx,
		kTraceStackDepth,
		&TraceStackData[0]
	);
	return bHitAny;
}
