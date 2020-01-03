#include "Camera.h"
#include "RenderView.h"
#include "math/Ray.h"
#include<algorithm>
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<stack>

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



float myRand() {
	return rand() / (RAND_MAX + 1.0);
}

Vec3 make_random(const Vec3 & dir) {
	// const Vec3 normal = 
	return (Vec3(myRand() - 0.5, myRand() - 0.5, myRand() - 0.5)).normalized();
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
	TraceRayBundle * bundle;
};


struct TraceRayBundle
{
	TraceRay traceRay[64];
	TraceRayBundle * parent;
	material * m;
	Vec3 colorSum;
	int depth;
	bool doneRequestRay;
};

bool Trace_(const Scene * s, 
			const std::vector<Shape*> * shapes, 
			const Ray & ray, 
			Vec3 & color, 
			void* memory){
	const int maxSample = 8;
	const int maxpDepth = 3;
	const int maxStack = 512;

	Vec3 hitPos;
	Vec3 direction;
	int idx;

	bool hit = RayCast(shapes, ray, hitPos, direction, idx);
	if (!hit) return false;

	int currentStackCount = 0;
	// TraceRayBundle * bundleStack = (TraceRayBundle*) malloc(sizeof(TraceRayBundle) * maxStack);
	TraceRayBundle * bundleStack = (TraceRayBundle*) memory;

	material * m = s->materials[idx];



	TraceRayBundle * rays = &bundleStack[currentStackCount];
	// memset(rays, 0, sizeof(TraceRayBundle));
	rays->colorSum = {0,0,0};
	rays->parent = NULL;
	rays->depth = 0;
	rays->m = m;
	rays->doneRequestRay = false;

	Vec3 normal = (-ray.dir + direction) / 2;
	for( int i = 0; i < maxSample ; i++ )
	{

		Vec3 dir = make_random(direction);
		while(Vec3::Dot(dir, normal) < 0) {
			dir = make_random(direction);
		}		
		//TODO use normal
		rays->traceRay[i].ray = {hitPos, dir};
		rays->traceRay[i].bundle = rays;
	}

	currentStackCount++;

	//TODO
	// TraceRayBundle * currentBundle;

	while(currentStackCount > 0) {
		TraceRayBundle * currentBundle = &bundleStack[currentStackCount-1];
		if (!currentBundle->doneRequestRay){

			//Handle Ray Bundle
			for ( int i = 0; i < maxSample; i++ ) {
				TraceRay traceRay = currentBundle->traceRay[i];
				bool hit = RayCast(shapes, traceRay.ray, hitPos, direction, idx);
					
				Vec3 normal = (-traceRay.ray.dir + direction) / 2;
				if (hit) {
					material * m = s->materials[idx];
					if (currentBundle->depth >= maxpDepth || m->emission.length() > 0.1) {
						//reach the end fallback
						currentBundle->colorSum += m->emission;
					}
					else {
						TraceRayBundle * rays = &bundleStack[currentStackCount];
						rays->colorSum = {0,0,0};
						rays->parent = currentBundle;
						rays->depth = currentBundle->depth + 1;
						rays->m = m;
						rays->doneRequestRay = false;
						for( int i = 0; i < maxSample ; i++ )
						{
							//TODO use normal
							Vec3 dir = make_random(direction);
							while(Vec3::Dot(dir, normal) < 0) {
								dir = make_random(direction);
							}

							rays->traceRay[i].ray = {hitPos, dir};
							rays->traceRay[i].bundle = rays;
						}

						currentStackCount++;
					}
				} else {
					//Not Hit
				}

				currentBundle->doneRequestRay = true;
			}

		} else {
			if (currentBundle->parent != NULL){
				material * m = currentBundle->m;
				currentBundle->parent->colorSum += (currentBundle->colorSum / maxSample) * m->color + m->emission;
			}
			else
				color = (currentBundle->colorSum / maxSample) * m->color + m->emission;

			currentStackCount--;
		}
	}


	// free(bundleStack);
	return true;
}

/*
bool Trace_(const Scene * s, const std::vector<Shape*> * shapes, const Ray & ray, Vec3 & color, bool bDebug){
	const int maxSample = 8;
	const int maxpDepth = 3;
	const int maxStack = 1024;

	int currentStackCount = 0;
	TraceRayBundle * bundleStack = (TraceRayBundle*) malloc(sizeof(TraceRayBundle) * maxStack);


	std::stack<TraceRayBundle*> traceStack;
	Vec3 hitPos;
	Vec3 direction;
	int idx;

	bool hit = RayCast(shapes, ray, hitPos, direction, idx);
	if (!hit) return false;

	material * m = s->materials[idx];

	TraceRayBundle * rays = new TraceRayBundle();
	rays->colorSum = {0,0,0};
	rays->parent = NULL;
	rays->depth = 0;
	rays->m = m;
	rays->doneRequestRay = false;

	Vec3 normal = (-ray.dir + direction) / 2;
	for( int i = 0; i < maxSample ; i++ )
	{

		Vec3 dir = make_random(direction);
		while(Vec3::Dot(dir, normal) < 0) {
			dir = make_random(direction);
		}		
		//TODO use normal
		rays->traceRay[i].ray = {hitPos, dir};
		rays->traceRay[i].bundle = rays;
	}

	traceStack.push(rays);	

	//TODO
	// TraceRayBundle * currentBundle;

	while(traceStack.size() > 0) {
		TraceRayBundle * currentBundle = traceStack.top();
		if (!currentBundle->doneRequestRay){

			//Handle Ray Bundle
			for ( int i = 0; i < maxSample; i++ ) {
				TraceRay traceRay = currentBundle->traceRay[i];
				bool hit = RayCast(shapes, traceRay.ray, hitPos, direction, idx);
					
				Vec3 normal = (-traceRay.ray.dir + direction) / 2;
				if (hit) {
					material * m = s->materials[idx];
					if (currentBundle->depth >= maxpDepth || m->emission.length() > 0.1) {
						//reach the end fallback
						currentBundle->colorSum += m->emission;
					}
					else {
						TraceRayBundle * rays = new TraceRayBundle();
						rays->colorSum = {0,0,0};
						rays->parent = currentBundle;
						rays->depth = currentBundle->depth + 1;
						rays->m = m;
						rays->doneRequestRay = false;
						for( int i = 0; i < maxSample ; i++ )
						{
							//TODO use normal
							Vec3 dir = make_random(direction);
							while(Vec3::Dot(dir, normal) < 0) {
								dir = make_random(direction);
							}

							rays->traceRay[i].ray = {hitPos, dir};
							rays->traceRay[i].bundle = rays;
						}

						traceStack.push(rays);
					}
				} else {
					//Not Hit
				}

				currentBundle->doneRequestRay = true;
			}

		} else {
			if (currentBundle->parent != NULL){
				material * m = currentBundle->m;
				currentBundle->parent->colorSum += (currentBundle->colorSum / maxSample) * m->color + m->emission;
			}
			else
				color = (currentBundle->colorSum / maxSample) * m->color + m->emission;

			traceStack.pop();
			delete currentBundle;
		}
	}

	return true;
}

*/


void Camera::RenderScenePixel(const Scene * s, int x, int y, void* memory) {
	const std::vector<Shape*> * shapes = &s->shapes;
	float width = view.GetWidth();
	float height = view.GetHeight();
	Ray ray = MakeRay(x / width, y/ height);

	Vec3 color;
	bool hit = Trace_(s, shapes, ray, color, memory);
	if(hit)
		view.SetPixel(x, view.GetHeight() - y - 1, EncodeInt32(color.x * 255, color.y * 255, color.z * 255, 255));

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
