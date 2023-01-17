#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

enum class TextureWrapping
{
	Repeat,
	Mirror,
	Clamp,
};

struct Texture
{
	int width;
	int height;
	TextureWrapping wrapping;
	std::vector<glm::vec4> pixels_data;
};


glm::vec4 SampleTexPixel(const Texture & texture, const glm::ivec2 & pixel);
glm::vec4 SampleTexPixel(const Texture & texture, int x, int y);
glm::vec4 SampleTex(const Texture & texture, const glm::vec2 & texCoord);
glm::ivec2 EvalTextureUV(const Texture & texture, const glm::vec2 & texCoord);



bool LoadTexture(std::string path, Texture & tex);
bool LoadExrTexture(std::string path, Texture & tex);