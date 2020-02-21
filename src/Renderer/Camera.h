#pragma once

#include "math/Quaternion.h"
#include "math/Transform.h"
#include "math/Vec3.h"
#include "RenderView.h"
#include "Scene.h"

class Camera {
public:
	Camera() : view(0, 0){}
	Camera( int width, int height, Vec3 position);

	Transform transform;

	void FullRender();
	unsigned int * GetBuffer();

	void RenderScene(const Scene * s);
	void RenderScenePixel(const Scene * s, int x, int y, void* memory, int itNum);

	void TryRayCast(const Scene * s, const Vec3 & origin, const Vec3 & dir);

	int GetWidth() {return view.GetWidth();}
	int GetHeight() {return view.GetHeight();}
	void * GetIntegrator() {return view.GetIntegrator();}
private:
	RenderView view;
	void RenderPixel_uv(float x, float y);
	void RenderPixel(int x, int y);

	Ray MakeRay(float x, float y);
};