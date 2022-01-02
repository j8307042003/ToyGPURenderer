#pragma once

#include "AppWindow.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <thread>
#include "AppWindow.h"

class AppWindowGLFW : public AppWindow
{
public:
	AppWindowGLFW();
	AppWindowGLFW(int width, int height);
	virtual ~AppWindowGLFW() {}
	virtual int getWidth() override { return m_width;}
	virtual int getHeight() override { return m_height;}
	virtual void SetSourceImage(int width, int height, char* buffer) override;
	virtual void SetEventCallback(const EventCallback & callback) override;
    virtual void* GetWindowHandle() override {return m_window;};
    virtual void SwapBuffer() override {glfwSwapBuffers(m_window);};

private:
	virtual void Update();

private:
	void init(int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);	

private:
	const int kDefaultWidth = 512;
	const int kDefaultHeight = 512;

private:
	// Window Resolution
	GLFWwindow* m_window;
	int m_width;
	int m_height;

	// Display Image
	char* m_sourceBuffer;
	int m_sourceWidth;
	int m_sourceHeight;

	EventCallback m_eventCallback;

	int m_textureId;
};
