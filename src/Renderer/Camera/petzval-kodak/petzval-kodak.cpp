#include "petzval-kodak.h"
#include "../../Random/SysRandom.h"
#include "lens.h"
#include <array>
#include <chrono>
#include <algorithm>
#include <iostream>

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

// Clamp value to the [0, 1] range
inline float clamp(float value, float mi, float ma) {
    return max(mi, min(ma, value));
}

// Convert wavelength (in nm) to RGB values
inline void wavelengthToRGB(float wavelength, float& r, float& g, float& b) {
    r = g = b = 0.0f; // Initialize RGB values

    wavelength = wavelength * 1000.0f;
    if (wavelength >= 380.0f && wavelength <= 440.0f) {
        r = -(wavelength - 440.0f) / (440.0f - 380.0f);
        g = 0.0f;
        b = 1.0f;
    }
    else if (wavelength > 440.0f && wavelength <= 490.0f) {
        r = 0.0f;
        g = (wavelength - 440.0f) / (490.0f - 440.0f);
        b = 1.0f;
    }
    else if (wavelength > 490.0f && wavelength <= 510.0f) {
        r = 0.0f;
        g = 1.0f;
        b = -(wavelength - 510.0f) / (510.0f - 490.0f);
    }
    else if (wavelength > 510.0f && wavelength <= 580.0f) {
        r = (wavelength - 510.0f) / (580.0f - 510.0f);
        g = 1.0f;
        b = 0.0f;
    }
    else if (wavelength > 580.0f && wavelength <= 645.0f) {
        r = 1.0f;
        g = -(wavelength - 645.0f) / (645.0f - 580.0f);
        b = 0.0f;
    }
    else if (wavelength > 645.0f && wavelength <= 780.0f) {
        r = 1.0f;
        g = 0.0f;
        b = 0.0f;
    }

    // Adjust intensity for wavelength-dependent sensitivity
    float intensity = 1.0f;
    if (wavelength >= 380.0f && wavelength < 420.0f) {
        intensity = 0.3f + 0.7f * (wavelength - 380.0f) / (420.0f - 380.0f);
    }
    else if (wavelength >= 645.0f && wavelength <= 780.0f) {
        intensity = 0.3f + 0.7f * (780.0f - wavelength) / (780.0f - 645.0f);
    }
    else if (wavelength < 380.0f || wavelength > 780.0f) {
        intensity = 0.0f;
    }

    // Apply intensity scaling and clamp to [0, 1]
    r = clamp(r * intensity, 0.0f, 1.0f);
    g = clamp(g * intensity, 0.0f, 1.0f);
    b = clamp(b * intensity, 0.0f, 1.0f);
}


Ray3f petzval_kodak_camera_data::SampleCamRay(const petzval_kodak_camera_model & cam, const vec3 & pos, const vec3 & direction, const vec2 & filmRes, const vec2 & pixelPos, vec3& transmittance, bool SimCam)
{
	auto r1 = SysRandom::Random();
	auto r2 = SysRandom::Random();
	auto r3 = SysRandom::Random();


    const float blue_lambda = 0.440f;
    const float green_lambda = 0.510f;
    const float red_lambda = 0.650f;

    const int rndLambdaIdx = int(r3 * 3.0f) % 3;
    std::array<float, 3> rgb_lambdas = {blue_lambda, green_lambda, red_lambda};
    wavelengthToRGB(rgb_lambdas[rndLambdaIdx], transmittance.x, transmittance.y, transmittance.z);
    transmittance *= 3.0f;
    //transmittance.x = transmittance.y = transmittance.z = 1.0f;

    //const float lambda = 0.4f + 0.3f * r3; //0.550f; // wavelength in um
    const float lambda = rgb_lambdas[rndLambdaIdx]; //0.550f; // wavelength in um
	const float dist = 10.0f;

	vec2 uv(pixelPos.x / filmRes.x, pixelPos.y / filmRes.y);
	uv -= 0.5f;

	vec2 filmPos_mm = uv * cam.film * 1000.0f;

	vec2 filmD_mm = -filmPos_mm / (lens_length - lens_aperture_pos + dist);

	float x = 0.0;
	float y = 0.0;
	lens_sample_aperture(&x, &y, r1, r2, 11.0, 5);

	std::array<float, 5> in = { {filmPos_mm.x, filmPos_mm.y, filmD_mm.x, filmD_mm.y, lambda} };
	glm::vec4 out = {0.0, 0.0, 0.0, 0.0};
	out.x = x;
	out.y = y;
	
	auto start = std::chrono::high_resolution_clock::now(); // End time

	lens_pt_sample_aperture(&in[0], &out[0], dist);
	auto end = std::chrono::high_resolution_clock::now(); // End time
	std::chrono::duration<double> elapsed = end - start; // Calculate duration

	glm::vec4 outer ={0.0, 0.0, 0.0, 0.0};

	glm::vec3 p = {};
	glm::vec3 d = {};
	auto start_1 = std::chrono::high_resolution_clock::now(); // End time
    float t = lens_evaluate(&in[0], &outer[0]);
    lens_sphereToCs(&outer[0], &outer[2], &p[0], &d.x, 0, 35.485001);
	auto end_1 = std::chrono::high_resolution_clock::now(); // End time
	std::chrono::duration<double> elapsed_1 = end_1 - start_1; // Calculate duration

	//std::cout << "sample aperture" << elapsed.count() << ". eval " << elapsed_1.count() << std::endl;

    d = glm::quatLookAt(direction, glm::vec3(0.0, 1.0, 0.0)) * -d;

    
    //wavelengthToRGB(lambda, transmittance.x, transmittance.y, transmittance.z);

	return Ray3f{ pos, d };
}