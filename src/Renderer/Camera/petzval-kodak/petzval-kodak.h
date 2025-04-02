#pragma once
#include <math/Ray.h>
#include "../Camera.h"


class petzval_kodak_camera_data {
	struct petzval_kodak_camera_model
	{
		float film;
	};


public:
	static Ray3f SampleCamRay(const petzval_kodak_camera_model& cam, const vec3& pos, const vec3& direction, const vec2& filmRes, const vec2& pixelPos, vec3 & transmittance, bool SimCam = false);
};
