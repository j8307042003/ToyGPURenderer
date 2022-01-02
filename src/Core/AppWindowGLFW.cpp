#include "AppWindowGLFW.h"
#include <functional>
#include <stdio.h>
#include <iostream>
#include "Event/WindowEvent.h"
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815

static int s_glfwInitCount = 0;

AppWindowGLFW::AppWindowGLFW()
{
	init(kDefaultWidth, kDefaultHeight);
}

AppWindowGLFW::AppWindowGLFW(int width, int height) : m_width(width), m_height(height)
{
	init(width, height);
}


void AppWindowGLFW::init(int width, int height)
{
	m_width = width;
	m_height = height;

	if (s_glfwInitCount == 0)
	{
		if (!glfwInit()) return;
		s_glfwInitCount++;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
	}

    m_window = glfwCreateWindow(m_width, m_height, "Renderer", NULL, NULL);

    std::cout << "Window Create : " << (m_window != nullptr) << std::endl;

    if (!m_window)
    {
        glfwTerminate();
        return;
    }

	glfwSetWindowUserPointer(m_window, this);

	auto f = std::bind(&AppWindowGLFW::mouse_button_callback, this, std::placeholders::_1);

	auto mouseButtonCallbackFunc = [](GLFWwindow* w, int button, int action, int mods)
	{
		static_cast<AppWindowGLFW*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
	};

	auto keyButtonCallbackFunc = [](GLFWwindow* w, int key, int scancode, int action, int mode)
	{
		static_cast<AppWindowGLFW*>(glfwGetWindowUserPointer(w))->key_callback(w, key, scancode, action, mode);
	};

    /* Make the window's context current */
    glfwMakeContextCurrent(m_window);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallbackFunc);
    glfwSetKeyCallback(m_window, keyButtonCallbackFunc);
    glfwSwapInterval(1);


	// Init
	glGenTextures(1, (GLuint*)&m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}


/*
AppWindowGLFW::~AppWindowGLFW()
{
    //glfwTerminate();
}
*/

void AppWindowGLFW::SetSourceImage(int width, int height, char* buffer)
{
	m_sourceBuffer = buffer;
	m_sourceWidth = width;
	m_sourceHeight = height;	
}

void AppWindowGLFW::SetEventCallback(const EventCallback & callback)
{
	m_eventCallback = callback;
}




void AppWindowGLFW::Update()
{
	/* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, m_textureId);

	glEnable(GL_TEXTURE_2D);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_sourceWidth, m_sourceHeight,
	            GL_RGB, GL_FLOAT, m_sourceBuffer);    
	            // GL_RGBA, GL_FLOAT, outputPtr);    
		        // GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());    
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(0, 1); glVertex2i(0, m_height);
	glTexCoord2i(1, 1); glVertex2i(m_width, m_height);
	glTexCoord2i(1, 0); glVertex2i(m_width, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


    /* Swap front and back buffers */
    //glfwSwapBuffers(m_window);

    /* Poll for and process events */
	glfwPollEvents();
}


void AppWindowGLFW::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //if(action == GLFW_PRESS) {
    //    double xpos, ypos;
    //    glfwGetCursorPos(window, &xpos, &ypos);
    //    std::cout << "-----------------" << std::endl;
    //    std::cout << std::endl << "x " << xpos << ". y " << ypos << std::endl;
    //    renderer->ClearImage();
    //    std::cout << "-----------------" << std::endl;
    //}

	/*
    if (action == GLFW_PRESS){
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_keys[button] = true;
    }
    else if (action == GLFW_RELEASE)
        mouse_keys[button] = false;
	*/


}

// Is called whenever a key is pressed/released via GLFW
void AppWindowGLFW::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	/*
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
	*/

	if (action == GLFW_PRESS)
	{
		KeyPressedEvent event = {};
		event.keyCode = key;
		if (m_eventCallback != nullptr) 
		{
			m_eventCallback(event);
		}
	}

}

