#pragma once
#include "Event/WindowEvent.h"
#include <functional>

class AppWindow
{
public:

	using EventCallback = std::function<void(WindowEvent&)>;

	virtual ~AppWindow() {};
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual void SetEventCallback(const EventCallback & callback) = 0;
	virtual void SetSourceImage(int width, int height, char* buffer) = 0;
	virtual void Update() = 0;
    virtual void* GetWindowHandle() = 0;
    virtual void SwapBuffer() = 0;
};

