#include "RenderView.h"
#include<stdlib.h>
#include<stdio.h>
#include<iostream>

RenderView::RenderView(int width, int height) : width(width), height(height) {
	std::cout << "Init Buff " << this->width << "  " << this->height << std::endl;
	InitBuffer();
}


void RenderView::InitBuffer() {
	if (buffer != NULL) {
		// delete buffer;
	}
	
	buffer = new unsigned int[width * height];
	color_r = new unsigned int[width * height];
	color_g = new unsigned int[width * height];
	color_b = new unsigned int[width * height];

	std::cout << "Buffer Size  " << width * height << std::endl;
}



void RenderView::SetPixel(int x, int y, int colorInt32 ) {
	//std::cout << x << "  " << y << std::endl;
	//std::cout << x * width + y << std::endl;
	buffer[x + y * width] = colorInt32;
}

void RenderView::AddPixel(int x, int y, int colorInt32) {
	buffer[x + y * width] += colorInt32;
}

int RenderView::GetPixel(int x, int y) {
	return buffer[x + y * width];
}

int RenderView::EncodeInt32(int r, int g, int b, int a)
{
	return std::min(255, r) + (std::min(g, 255) << 8) + (std::min(b, 255) << 16) + (std::min(a, 255) << 24);
}

void RenderView::AddPixelSample(int x, int y, int itNum, float r, float g, float b) {
	color_r[x + y * width] += r * 255;
	color_g[x + y * width] += g * 255;
	color_b[x + y * width] += b * 255;

	SetPixel(x, y, 
		EncodeInt32(
			color_r[x + y * width] / itNum,
			color_g[x + y * width] / itNum,
			color_b[x + y * width] / itNum,
			255			
		));
}

