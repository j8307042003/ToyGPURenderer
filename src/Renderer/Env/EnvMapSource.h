#include "IEnvSource.h"
#include "../Texture/Texture.h"

class EnvMapSource : public IEnvSource
{
public:
	Texture* envTexture;
	float scale = 1.0f;


	const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
	glm::vec2 SampleSphericalMap(glm::vec3 v)
	{
	    glm::vec2 uv = glm::vec2(glm::atan(v.z, v.x), glm::asin(v.y));
	    uv *= invAtan;
	    uv += 0.5;
	    uv.y = 1 - uv.y;
	    return uv;
	}

	virtual glm::vec3 Sample(const glm::dvec3 & direction)
	{
		auto uv = SampleSphericalMap(direction);

		auto pixData = SampleTex(*envTexture, uv);
		return glm::vec3(pixData) * scale;
	}
};