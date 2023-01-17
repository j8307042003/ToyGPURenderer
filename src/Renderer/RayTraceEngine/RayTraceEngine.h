#pragma once
#include "IRayTraceEngine.h"



class RayTraceEngine : public IRayTraceEngine
{
public:
	virtual bool IntersectScene(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max, SceneIntersectData* intersect) override;
	virtual bool Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max) override;
};
