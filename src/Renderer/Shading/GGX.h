#pragma once
#include <algorithm>


class GGX
{
public:
	static float GGXAnisotropicD(glm::vec3 wm, float ax, float ay)
	{
		float dotHX2 = wm.x * wm.x;
		float dotHY2 = wm.y * wm.y;
		float cos2Theta = glm::dot(wm, wm);
		float ax2 = ax * ax;
		float ay2 = ay * ay;
		float pi = glm::pi<float>();
		return 1.0 / (pi * ax * ay * square((dotHX2 / ax2) + (dotHY2 / ay2) + cos2Theta));
	}

	static float GGXAnisotropicG1(const glm::vec3 & w, const glm::vec3 & wm)
	{
		float dotHW = glm::dot(w, wm);
		if (dotHW < 0.0) return 0.0f;

		float absTan = std::abs(glm::tan(w));
		if (isinf(absTan)) return 0.0f;


		float a = sqrt(CosPhi(w) * ax * ax + SinPhi(w) * ay * ay);
		float a2Tan2Theta = square(a * absTan);

		float A = (-1 + sqrt(1 + a2Tan2Theta)) * 0.5f;
		return 1.0 / (1.0 + A);
	}

	static float SeparableSmithGGXG1(const glm::vec3& w, const glm::vec3& wm, float ax, float ay)
	{
	    // JSTODO - Why did I have this here?
	    //float dotHW = Dot(w, wm);
	    //if (dotHW <= 0.0f) {
	    //    return 0.0f;
	    //}

	    float absTanTheta = std::abs(glm::tan(w));
	    if(isinf(absTanTheta)) {
	        return 0.0f;
	    }

	    float a = sqrt(Cos2Phi(w) * ax * ax + Sin2Phi(w) * ay * ay);
	    float a2Tan2Theta = square(a * absTanTheta);

	    float lambda = 0.5f * (-1.0f + std::sqrtf(1.0f + a2Tan2Theta));
	    return 1.0f / (1.0f + lambda);
	}


	static void GgxVndfAnisotropicPdf(const glm::vec3& wi, const glm::vec3& wm, const glm::vec3& wo, float ax, float ay,
	                           float& forwardPdfW, float& reversePdfW)
	{
	    float D = GGXAnisotropicD(wm, ax, ay);

	    float absDotNL = AbsCosTheta(wi);
	    float absDotHL = std::abs(glm::dot(wm, wi));
	    float G1v = SeparableSmithGGXG1(wo, wm, ax, ay);
	    forwardPdfW = G1v * absDotHL * D / absDotNL;

	    float absDotNV = AbsCosTheta(wo);
	    float absDotHV = std::abs(glm::dot(wm, wo));
	    float G1l = SeparableSmithGGXG1(wi, wm, ax, ay);
	    reversePdfW = G1l * absDotHV * D / absDotNV;
	}
	
};