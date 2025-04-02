#pragma once
#include <math/Ray.h>
#include "../Camera.h"


class petzval_camera_data {

public:
	static Ray3f SampleCamRay(const CameraData& cam, const vec3& pos, const vec3& direction, const vec2& filmRes, const vec2& pixelPos, vec3 & transmittance, bool staticRay = false);
};
