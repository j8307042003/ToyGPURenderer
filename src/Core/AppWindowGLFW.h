#pragma once

#include "AppWindow.h"
#define GL_SILENCE_DEPRECATION
#include <glad.h>
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
	virtual void GetMousePos(float & x, float & y) override;
	virtual void SetSourceImage(int width, int height, char* buffer, ColorFormat format) override;
	virtual void SetEventCallback(const EventCallback & callback) override;
    virtual bool WindowShouldClose() override;
    virtual void* GetWindowHandle() override {return m_window;};
    virtual void SwapBuffer() override {glfwSwapBuffers(m_window);};

private:
	virtual void Update();

private:
	void init(int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);	
	void renderQuad(int textureId);

private:
	struct GLColorFormatResult
	{
		GLenum colorChannel;
		GLenum format;
	};

	GLColorFormatResult getGLFormat(ColorFormat format);

private:
	const int kDefaultWidth = 512;
	const int kDefaultHeight = 512;

private:
	// Window Resolution
	GLFWwindow* m_window;
	int m_width;
	int m_height;

	// OpenGL Data
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	GLuint m_screenQuadShader = 0;

	// Display Image
	char* m_sourceBuffer;
	int m_sourceWidth;
	int m_sourceHeight;
	ColorFormat m_colorFormat;

	EventCallback m_eventCallback;

	GLuint m_textureId;
};
