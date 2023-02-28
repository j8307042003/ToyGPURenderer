#include "Texture.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "../../assimp/contrib/stb/stb_image.h"
#define TINYEXR_IMPLEMENTATION
#define TINYEXR_USE_MINIZ 0
#include <miniz/miniz.h>
#include "tinyexr.h"

glm::vec4 SampleTexPixel(const Texture & texture, const glm::ivec2 & pixel)
{
	return SampleTexPixel(texture, pixel.x, pixel.y);
}

glm::vec4 SampleTexPixel(const Texture & texture, int x, int y)
{
	int offset = y * texture.width + x;
	glm::vec4 color = texture.pixels_data[offset]; 
	return color;
}


glm::vec4 SampleTex(const Texture & texture, const glm::vec2 & texCoord)
{	
	//glm::ivec2 pixel = texCoord * glm::vec2(texture.width, texture.height);
	//pixel = glm::clamp(pixel, glm::ivec2(0, 0), glm::ivec2(texture.width-1, texture.height-1));
	glm::ivec2 pixel = EvalTextureUV(texture, texCoord);
	return SampleTexPixel(texture, pixel);
}

glm::ivec2 EvalTextureUV(const Texture & texture, const glm::vec2 & texCoord)
{
	glm::ivec2 result = {};
	glm::ivec2 pixel = texCoord * glm::vec2(texture.width, texture.height);	
	switch(texture.wrapping)
	{
		case TextureWrapping::Repeat: 
			result = (texCoord - glm::floor(texCoord)) * glm::vec2(texture.width, texture.height); 
			break;
		case TextureWrapping::Mirror:
		{
			auto floor = glm::floor(texCoord);
			auto frac = texCoord - floor;
			bool mirrorX = int(floor.x) % 2 == 0;
			bool mirrorY = int(floor.y) % 2 == 0;
			result = glm::ivec2(mirrorX ? 1.0f - frac.x : frac.x, mirrorY ? 1.0f - frac.y : frac.y);
			break;
		}
		case TextureWrapping::Clamp: 
			result = glm::clamp(pixel, glm::ivec2(0, 0), glm::ivec2(texture.width-1, texture.height-1)); 
			break;
	}

	return result;
}



bool LoadTexture(std::string path, Texture & tex)
{
	int width, height, channels;	
	unsigned char * imageFileData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	if (imageFileData == nullptr) return false;

	tex.width = width;
	tex.height = height;
	tex.pixels_data = std::vector<glm::vec4>(width * height);

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			int pix = (j * width + i);
			int offset = pix * 4;

			tex.pixels_data[pix] = { pow(imageFileData[offset] / 255.0f, 2.2f), pow(imageFileData[offset+1] / 255.0f, 2.2) , pow(imageFileData[offset+2] / 255.0f, 2.2), pow(imageFileData[offset+3] / 255.0f, 2.2) };
		}
	}

	stbi_image_free(imageFileData);

	return true;
}

bool LoadExrTexture(std::string path, Texture & tex)
{
	float* data;
	int width, height;
	const char* err = NULL;
	int ret = LoadEXR(&data, &width, &height, path.c_str(), &err);

	if (ret != TINYEXR_SUCCESS)
	{
		return false;
	}	

	tex.width = width;
	tex.height = height;
	tex.pixels_data = std::vector<glm::vec4>(width * height);
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			int pix = (j * width + i);
			int offset = pix * 4;

			tex.pixels_data[pix] = { pow(data[offset], 2.2), pow(data[offset+1], 2.2) , pow(data[offset+2], 2.2), data[offset+3] };
		}
	}	

	return true;
}