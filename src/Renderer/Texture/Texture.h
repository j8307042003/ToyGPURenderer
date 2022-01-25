#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Texture
{
	int width;
	int height;
	std::vector<glm::vec4> pixels_data;
};


glm::vec4 SampleTexPixel(const Texture & texture, const glm::ivec2 & pixel);
glm::vec4 SampleTexPixel(const Texture & texture, int x, int y);
glm::vec4 SampleTex(const Texture & texture, const glm::vec2 & texCoord);




bool LoadTexture(std::string path, Texture & tex);