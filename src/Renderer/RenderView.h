#pragma once

#include"math/Vec3.h"
#include<stdlib.h>
#include<stdio.h>
#include<iostream>



class RenderView {
public:
	RenderView(int width, int height);

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	Vec3 GetUV(int x, int y) {
		return Vec3( (float)x / width, (float)y / height, 0.0 );
	}

	void SetPixel(int x, int y, int colorInt32);
	unsigned int * GetBuffer() { return buffer; }
private:
	int width;
	int height;

	void InitBuffer();

	unsigned int * buffer;
};