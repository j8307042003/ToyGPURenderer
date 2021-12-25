#pragma once



class AppWindow
{
public:
	virtual ~AppWindow() {};
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;

	virtual void SetSourceImage(int width, int height, char* buffer) = 0;
	virtual void Update() = 0;
};

