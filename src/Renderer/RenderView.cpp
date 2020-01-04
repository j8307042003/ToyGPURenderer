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
		delete buffer;
	}
	
	buffer = new unsigned int[width * height];
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

