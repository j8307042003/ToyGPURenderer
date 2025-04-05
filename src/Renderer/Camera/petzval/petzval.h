#pragma once
#include <math/Ray.h>
#include "../Camera.h"

struct PetzvalCamData
{
	float film;
	float aperture = 1.0f;
	float dist = 0.0f;
};

class petzval_camera_data {

public:
	static CameraData MakeCamData();
	static Ray3f SampleCamRay(const PetzvalCamData& cam, const vec3& pos, const vec3& direction, const vec2& filmRes, const vec2& pixelPos, vec3 & transmittance, bool staticRay = false);
};
