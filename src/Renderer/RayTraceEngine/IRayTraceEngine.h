#ifndef  IRAYTRACEENGINE_H
#define IRAYTRACEENGINE_H
#pragma once

#include "math/Ray.h"

struct SceneData;
struct SceneIntersectData;

class IRayTraceEngine
{
public:
	virtual bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData * intersect) = 0;
	virtual bool Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max) = 0;
};
#endif // ! IRAYTRACEENGINE_H
