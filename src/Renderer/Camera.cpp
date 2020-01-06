#include "Camera.h"
#include "RenderView.h"
#include "math/Ray.h"
#include<algorithm>
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<stack>
#include<cmath>

Vec3 normalize(Vec3 v) {
	return v.normalized();
}


const auto vec3Zero = Vec3{0, 0, 0};

int EncodeInt32(int r, int g, int b, int a)
{
	return std::min(255, r) + (std::min(g, 255) << 8) + (std::min(b, 255) << 16) + (std::min(a, 255) << 24);
}

Camera::Camera( int width, int height, Vec3 position) : 
	view(width, height),
	// transform(position, Quaternion(Vec3(0, 1, 0), 30))
	transform(position, Quaternion())
{
	std::cout << "Init Camera";
	//view = RenderView(width, height);
}


void make_coordinateSys(const Vec3 & normal, Vec3 & normal_t, Vec3 & normal_b) {
	if (std::fabs(normal.x) > std::fabs(normal.y))
		normal_t = {normal.z, 0, -normal.x};
	else
		normal_t = {0, -normal.z, normal.y};


	normal_b = Vec3::Cross(normal, normal_t);		
}

Vec3 uniformSampleHemisphere(const float &r1, const float &r2) 
{ 
    // cos(theta) = r1 = y
    // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
    float sinTheta = sqrtf(1 - r1 * r1); 
    float phi = 2 * M_PI * r2; 
    float x = sinTheta * cosf(phi); 
    float z = sinTheta * sinf(phi); 
    return Vec3(x, r1, z); 
} 
 



float myRand() {
	return rand() / (RAND_MAX + 1.0);
}

float Rand01() {
	return rand() / (RAND_MAX + 1.0);
}

Vec3 make_random(const Vec3 & dir) {
	// const Vec3 normal = 
	// return (Vec3(myRand() - 0.5, myRand() - 0.5, myRand() - 0.5)).normalized();
	Vec3 Nb, Nt;
	make_coordinateSys(dir, Nb, Nt);

	float r1 = Rand01();
	float r2 = Rand01();

	Vec3 localHemiSphereDir = uniformSampleHemisphere(r1, r2);
    return { 
        localHemiSphereDir.x * Nb.x + localHemiSphereDir.y * dir.x + localHemiSphereDir.z * Nt.x, 
        localHemiSphereDir.x * Nb.y + localHemiSphereDir.y * dir.y + localHemiSphereDir.z * Nt.y, 
        localHemiSphereDir.x * Nb.z + localHemiSphereDir.y * dir.z + localHemiSphereDir.z * Nt.z}; 	
}

void Camera::FullRender() {
	float width = view.GetWidth();
	float height = view.GetHeight();

	for (int i = 0 ; i < view.GetWidth(); i++) {
		for (int j = 0 ; j < view.GetHeight(); j++) {
			RenderPixel(i, j);
  			// view.SetPixel(i, j, EncodeInt32((i / (float) width) * 255, (j / (float) height) * 255, 1 * 255, 255));

		}
	}
}

bool RayCast(const std::vector<Shape*> * shapes, const Ray & ray, Vec3 & hitPos, Vec3 & direction, int & idx) {
	float depth = 9999999;
	bool bEverHit = false;
	Vec3 pos;
	Vec3 dir;
	for(int k = 0 ; k < shapes->size() ; k++){
		bool bHit = (*shapes)[k]->RayCastTest(&ray, pos, dir);
		//lazy depth test
		float hitDepth = (pos - ray.origin).length();

		if (bHit && hitDepth < depth ) {
			depth = hitDepth;
			hitPos = pos;
			idx = k;
			direction = dir;
			bEverHit = true;
		}
	}

	return bEverHit;
}

bool Trace(const Scene * s, const std::vector<Shape*> * shapes, const Ray & ray, Vec3 & color, int depth, bool bDebug) {
	if(depth == 0) return false;

	Vec3 hitPos, direction;
	int idx;
	bool hit = RayCast(shapes, ray, hitPos, direction, idx);
	if (hit) {
		if(bDebug) 
		{
			std::cout << "Hit!" << std::endl;
			std::cout << "hit id " << idx << std::endl;
			std::cout << "Ray Origin " << ray.origin.tostring() << std::endl;
			std::cout << "Ray Dir " << ray.dir.tostring() << std::endl;
			std::cout << "Ray Hits pos " << hitPos.x << " " << hitPos.y << " " << hitPos.z << 
				".  Dir " << direction.x << " " << direction.y << " " << direction.z << std::endl;
		}
		// std::cout << "Hits " << depth << std::endl;
		Vec3 traceColor;
		material m = *s->materials[idx];
		/*
		if (m.emission.length() > 0.001) {
			color = m.emission;
			if(bDebug)
				std::cout << "Hit emission Color " << m.emission.x << "  " << m.emission.y << "  " << m.emission.z << std::endl; 
			return true;
		}
		*/
		bool bTraceHit = Trace(s, shapes, {hitPos, direction}, traceColor, depth - 1, bDebug);
		color = bTraceHit ? m.emission + (m.emission + m.color) * traceColor : m.emission;
		// color = bTraceHit ? m.color + m.color * traceColor : m.color;
		// color = m.color;
		if(bDebug)
			std::cout << "Hit Color " << color.x << "  " << color.y << "  " << color.z << std::endl; 

		return true;
	} else {
		return false;
	}
}

struct TraceRayBundle;

struct TraceRay {
	Ray ray;
	material * m;
	TraceRayBundle * bundle;
};


struct TraceRayBundle
{
	TraceRay traceRay[64];
	TraceRayBundle * parent;
	int parentIdx;
	material * m;
	Vec3 dir;
	Vec3 normal;
	Vec3 colorSum;
	int depth;
	bool doneRequestRay;
};

void ResolveRayBundle(TraceRayBundle & bundle, int maxSample)
{
	// bundle.colorSum = {0,0,0};
	for (int i = 0; i < maxSample; i++) {
		const TraceRay & ray = bundle.traceRay[i];
		if (ray.m == NULL) continue;
		Vec3 diffuse = ray.m->emission * bundle.m->color;

		Vec3 halfwayDir = ( ray.ray.dir + bundle.dir ).normalized();
		Vec3 spec = ray.m->emission * Vec3::Dot( bundle.normal, halfwayDir );
		bundle.colorSum += diffuse * (1) + spec * (bundle.m->specular);
	}

	bundle.colorSum = bundle.colorSum / maxSample;
}

void ResolveRayBundleParent(TraceRayBundle & bundle)
{
	TraceRayBundle * parent = bundle.parent;
	if (parent == NULL) return;
	Vec3 diffuse = bundle.colorSum * parent->m->color;

	Vec3 halfwayDir = ( bundle.dir + parent->dir ).normalized();
	Vec3 spec = parent->m->emission * pow( fmax( Vec3::Dot( parent->normal, halfwayDir ), 0 ), 32.0 );
	parent->colorSum += diffuse * (1) + spec * (parent->m->specular);	
}


struct RayHitInfo {
	Vec3 dir;
	Vec3 normal;
	Vec3 reflect;
	material * m;
	Vec3 color;
};

void ResolveRayHit(RayHitInfo * rayHitStack, int depth) {
	for(int i = depth; i >= 0; --i) {
		RayHitInfo * rayHitInfo = &rayHitStack[i];
		RayHitInfo * subRayHitInfo = &rayHitStack[i+1];
		rayHitInfo->color = rayHitInfo->m->emission + (i == depth ? Vec3(0, 0, 0) : subRayHitInfo->color * rayHitInfo->m->color);
	}
}

bool Trace_(const Scene * s, 
			const std::vector<Shape*> * shapes, 
			const Ray & ray, 
			Vec3 & color, 
			void* memory){
	const int maxSample = 2;
	const int maxpDepth = 10;
	const int maxStack = 512;

	Vec3 hitPos;
	Vec3 direction;
	int idx;


	color = {0, 0, 0};
	// TraceRayBundle * bundleStack = (TraceRayBundle*) memory;
	RayHitInfo * rayHitInfoStack = (RayHitInfo*) memory;

	Ray traversalRay = ray;

	int depth = 0;
	for(;; depth++) {
		bool hit = RayCast(shapes, traversalRay, hitPos, direction, idx);
		if (!hit) {
			if (depth == 0) return false;
			else {
				ResolveRayHit(rayHitInfoStack, depth-1);
				color = rayHitInfoStack[0].color;				
				break;
			}
		}

		material *m  = s->materials[idx];
		Vec3 normal = (-traversalRay.dir + direction) / 2;
		RayHitInfo * rayHitInfo = &rayHitInfoStack[depth];
		rayHitInfo->m = m;
		rayHitInfo->dir = traversalRay.dir;
		rayHitInfo->normal = normal;
		rayHitInfo->reflect = direction;
		rayHitInfo->color = {0, 0, 0};

		if (m->emission.length() > 0.8 || depth == maxpDepth) {
			ResolveRayHit(rayHitInfoStack, depth);
			color = rayHitInfoStack[0].color;
			break;
		}

		traversalRay.dir = make_random(normal);
		traversalRay.origin = hitPos;
	}


	return true;
}


void Camera::RenderScenePixel(const Scene * s, int x, int y, void* memory, int itNum) {
	const std::vector<Shape*> * shapes = &s->shapes;
	float width = view.GetWidth();
	float height = view.GetHeight();
	Ray ray = MakeRay(x / width, y/ height);

	Vec3 color;
	bool hit = Trace_(s, shapes, ray, color, memory);

	// int pixelColor = view.GetPixel(x, view.GetHeight() - y - 1);
	// Vec3 vPixelColor = { (pixelColor & 255) / 255.0f, (pixelColor >> 8 & 255) / 255.0f, (pixelColor >> 16 & 255) / 255.0f };
	// vPixelColor = (vPixelColor * (itNum - 1 ) + (hit ? color : Vec3(0,0,0))) / itNum;

	// int r = ((pixelColor & 255) * (itNum - 1 ) + (hit ? color.x * 255 : 0)) / itNum;
	// int g = ((pixelColor >>  8 & 255) * (itNum - 1 ) + (hit ? color.y * 255 : 0)) / itNum;
	// int b = ((pixelColor >> 16 & 255) * (itNum - 1 ) + (hit ? color.z * 255 : 0)) / itNum;
	// int a = ((pixelColor >> 24 & 255) * (itNum - 1 ) + (hit ? color.x * 255 : 0)) / itNum;

	if(hit){
		// view.SetPixel(x, view.GetHeight() - y - 1, EncodeInt32(vPixelColor.x * 255, vPixelColor.y * 255, vPixelColor.z * 255, 255));
		view.AddPixelSample(x, view.GetHeight() - y - 1, itNum, color.x, color.y, color.z);

		// view.SetPixel(x, view.GetHeight() - y - 1, EncodeInt32(r, g, b, 255));
	}

}


void Camera::RenderScene(const Scene * s) {
	float width = view.GetWidth();
	float height = view.GetHeight();

	const int maxStack = 512;
	void* bundleStack = malloc(sizeof(TraceRayBundle) * maxStack);

	const std::vector<Shape*> * shapes = &s->shapes;
	Vec3 hitPos, direction;
	for (int i = 0 ; i < view.GetWidth(); i++) {
		for (int j = 0 ; j < view.GetHeight(); j++) {
			Ray ray = MakeRay(i / width, j/ height);

			int idx;
			// bool hit = RayCast(shapes, ray, hitPos, direction, idx);

			Vec3 color;
			bool bDebug = (i == 159 && j == 35);
			// bool hit = Trace(s, shapes, ray, color, 8, bDebug);
			bool hit = Trace_(s, shapes, ray, color, bundleStack);

			if(bDebug)
				std::cout << "final Color " << color.x << " " << color.y << "  " << color.z << std::endl;

			//Vec3 pos;
			//float depth = 9999999;
			//for(int k = 0 ; k< shapes->size() ; k++){
			//	bool bHit = (*shapes)[k]->RayCastTest(&ray, pos);
			//	//lazy depth test
			//	float hitDepth = (pos - ray.origin).length();
//
//			//	pos = pos / 10.0f;
//			//	pos = pos * 0.5 + 0.5;
//			//	if (bHit && hitDepth < depth ) {
//			//		depth = hitDepth;
//			//	}
			//}
			if(hit) {
				// material m = *(s->materials[idx]);
				view.SetPixel(i, view.GetHeight() - j - 1, EncodeInt32(color.x * 255, color.y * 255, color.z * 255, 255));
			}
		}
	}	
}


void Camera::TryRayCast(const Scene * s, const Vec3 & origin, const Vec3 & dir) {
	Ray ray = {origin, dir};
	const std::vector<Shape*> * shapes = &s->shapes;
	Vec3 color;
	bool hit = Trace(s, shapes, ray, color, 32, true);
	std::cout << "Hit? : " << hit;
	std::cout << "final Color " << color.x << " " << color.y << "  " << color.z << std::endl;

}


unsigned int * Camera::GetBuffer() {
	return view.GetBuffer();
}


void Camera::RenderPixel_uv(float x, float y) {
	Vec3 uv(x, y, 0);
	const float film = 0.036f;
	const float lens = 0.05f;
	auto e = vec3Zero;
  	auto q = Vec3{
      			film * (0.5f - uv.x), film / (view.GetWidth() / (float)view.GetHeight()) * (uv.y - 0.5f), lens
      		 };
  	auto q1  = -q;
  	auto d   = normalize(q1 - e);
  	d = d * 0.5f + 0.5f;
  	auto ray = Ray{transform.TransformPoint(e), transform.TransformDir(d)};
  	//return ray;
  	// view:SetPixel(x * view.GetWidth(), y * view.GetHeight(), EncodeInt32(d.x * 255, d.y * 255, d.z * 255, 255));
  	//view.SetPixel(1, 1, EncodeInt32(d.x * 255, d.y * 255, d.z * 255, 255));
  	view.SetPixel(x * view.GetWidth(), y * view.GetHeight(), EncodeInt32(d.x * 255, d.y * 255, d.z * 255, 255));
  	view.SetPixel(x * view.GetWidth(), y * view.GetHeight(), EncodeInt32(x * 255, y * 255, d.z * 255, 255));
  	view.SetPixel(x * view.GetWidth(), y * view.GetHeight(), EncodeInt32(ray.dir.x * 255, ray.dir.y * 255, d.z * 255, 255));
}


void Camera::RenderPixel(int x, int y){
	Vec3 uv = view.GetUV(x, y);
	RenderPixel_uv(uv.x, uv.y);
}


Ray Camera::MakeRay(float x, float y) {
	Vec3 uv(x, y, 0);
	const float film = 0.036f;
	const float lens = 0.05f;
	auto e = vec3Zero;
  	auto q = Vec3{
      			// film * (0.5f - uv.x), film * (uv.y - 0.5f), lens
      			film * (0.5f - uv.x), film / (view.GetWidth() / (float)view.GetHeight()) * (uv.y - 0.5f), lens

      		 };
  	auto q1  = -q;
  	auto d   = normalize(q1 - e);
  	// d.z = -d.z;
  	// d.y = -d.y;
  	// d = d * 0.5f + 0.5f;
  	// auto ray = Ray{transform.TransformPoint(e), transform.TransformDir(d)};
  	auto ray = Ray{e, (d)};
  	return ray;
}
