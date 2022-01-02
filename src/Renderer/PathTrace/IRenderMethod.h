#pragma once


struct RenderData
{
	// Camera
	// Scene Geometry
	// Resolution		
};

struct TileData
{
	int x;
	int y;

	int width;
	int height;
};

class IRenderMethod
{
public:
	virtual ~IRenderMethod() = default;
	virtual void Sample(RenderData * rdrData, int x, int y) = 0;
};