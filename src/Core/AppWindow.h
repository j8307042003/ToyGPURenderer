#pragma once
#include "Event/WindowEvent.h"
#include <functional>
#include "Renderer/Color/ColorFormat.h"

class AppWindow
{
public:

	using EventCallback = std::function<void(WindowEvent&)>;

	virtual ~AppWindow() {};
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual void SetEventCallback(const EventCallback & callback) = 0;
    virtual bool WindowShouldClose() {};
	virtual void SetSourceImage(int width, int height, char* buffer, ColorFormat format = ColorFormat::RGBFloat) = 0;
	virtual void GetMousePos(float& x, float& y) = 0;
	virtual void Update() = 0;
    virtual void* GetWindowHandle() = 0;
    virtual void SwapBuffer() = 0;
};

