#ifndef  IRAYTRACEENGINE_H
#define IRAYTRACEENGINE_H
#pragma once

#include "math/Ray.h"

struct SceneData;
struct SceneIntersectData;
struct DynamicMesh;

class IRayTraceEngine
{
public:
	virtual bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData * intersect) = 0;
	virtual bool Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max) = 0;
    virtual bool AddDynamicGeometry(DynamicMesh * model) { return false;}
    virtual bool UpdateDynamicGeometry(DynamicMesh * model) { return false;}
};
#endif // ! IRAYTRACEENGINE_H
