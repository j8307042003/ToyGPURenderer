#ifndef EMBREEENGINE_H
#define EMBREEENGINE_H

#pragma once
#include "IRayTraceEngine.h"
#include <embree3/rtcore.h>
#include <math/Ray.h>

class EmbreeEngine : public IRayTraceEngine
{
public:
	static EmbreeEngine* BuildEmgreeEngine(SceneData * sceneData);

public:
	RTCDevice device;
	RTCScene scene;
	RTCGeometry geom;

private:
	SceneData* m_sceneData;

public:
	virtual bool IntersectScene(SceneData * sceneData, const Ray3f & ray, float t_min, float t_max, SceneIntersectData * intersect) override;
	virtual bool Raycast(SceneData* sceneData, const Ray3f& ray, float t_min, float t_max) override;

public:
	static SceneData* s_sceneData;
	static void IntersectFilter(const RTCFilterFunctionNArguments* args); // TODO
};

#endif