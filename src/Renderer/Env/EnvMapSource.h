#include "IEnvSource.h"
#include "../Texture/Texture.h"

class EnvMapSource : public IEnvSource
{
public:
	Texture* envTexture;
	float scale = 1.0f;
	float sampleScale = 1.0f;

	const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
	glm::vec2 SampleSphericalMap(glm::vec3 v)
	{
	    glm::vec2 uv = glm::vec2(glm::atan(v.z, v.x), glm::asin(v.y));
	    uv *= invAtan;
	    uv += 0.5;
	    uv.y = 1 - uv.y;
	    return uv;
	}

	virtual glm::vec3 Sample(const glm::dvec3 & direction, bool bEye)
	{
		auto uv = SampleSphericalMap(direction);

		auto tex_uv = uv * glm::vec2(envTexture->width, envTexture->height);
		auto iuv = glm::ivec2(glm::floor(tex_uv));
		int x1 = iuv.x + 1;
		int y1 = iuv.y + 1;

		float sx = tex_uv.x - iuv.x;
		float sy = tex_uv.y - iuv.y;


		//auto pixData = SampleTex(*envTexture, uv);

		float dx = 1.0 / envTexture->width;
		float dy = 1.0 / envTexture->height;


		auto pixData_00 = SampleTex(*envTexture, uv);
		auto pixData_01 = SampleTex(*envTexture, uv + glm::vec2(0.0f, dy));
		auto pixData_10 = SampleTex(*envTexture, uv + glm::vec2(dx, 0.0));
		auto pixData_11 = SampleTex(*envTexture, uv + glm::vec2(dx, dy));

		auto c0 = pixData_00 * (1 - sx) + pixData_10 * sx;
		auto c1 = pixData_01 * (1 - sx) + pixData_11 * sx;

		auto pixData = c0 * (1 - sy) + c1 * sy;

		//auto pixData_0 = SampleTex(*envTexture, uv + glm::vec2(0.0f, 0.0f) );
		//auto pixData_1 = SampleTex(*envTexture, uv + glm::vec2(0.0f, 0.1f) );
		//auto pixData_2 = SampleTex(*envTexture, uv + glm::vec2(0.1f, 0.0f) );
		//auto pixData_3 = SampleTex(*envTexture, uv + glm::vec2(0.1f, 0.1f) );

		//pixData = pixData * 0.6f + (pixData_0 + pixData_1 + pixData_2 + pixData_3) * 0.1f;

		return glm::vec3(pixData) * (bEye ? scale : sampleScale);
		//return glm::vec3(pixData) * scale;
	}
};
