#include "AppWindowGLFW.h"
#include <functional>
#include <stdio.h>
#include <iostream>
#include "Event/WindowEvent.h"
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815

static int s_glfwInitCount = 0;

AppWindowGLFW::AppWindowGLFW() : m_textureId(0)
{
	init(kDefaultWidth, kDefaultHeight);
}

AppWindowGLFW::AppWindowGLFW(int width, int height) : m_width(width), m_height(height), m_window(nullptr), m_textureId(0)
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
        
        auto errorCallback = [](int code, const char* description)
        {
            std::cout << "error: %s\n" << description << std::endl;
        };
        
        glfwSetErrorCallback(errorCallback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
        glfwWindowHint(GLFW_CONTEXT_DEBUG, GL_TRUE);
        glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);
		//gladLoadGL();
		
	}

	this->m_window = glfwCreateWindow(m_width, m_height, "Renderer", NULL, NULL);

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
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// Init
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	{
		const char* screeenVertexShader = R"(#version 330 core
			layout(location = 0) in vec2 position;
		layout(location = 1) in vec2 texCoords;

		out vec2 TexCoords;

		void main()
		{
			gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
			TexCoords = texCoords;
		}  )";

		const char* screenFragShader = R"(
			#version 330 core
			in vec2 TexCoords;
			out vec4 color;

			uniform sampler2D screenTexture;

			void main()
			{ 
				color = texture(screenTexture, TexCoords) ;
			})";

		GLuint vertex, fragment, geometry;
		GLint success;
		GLuint program;
		GLchar infoLog[512];
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &screeenVertexShader, NULL);
		glCompileShader(vertex);

		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &screenFragShader, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		}

		m_screenQuadShader = program;
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);


		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
	}

	glViewport(0, 0, width, height);        // Reset The Current Viewport
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/
    
    std::cout << "gl error : " << glGetError() << std::endl;
}



/*
AppWindowGLFW::~AppWindowGLFW()
{
    //glfwTerminate();
}
*/

void AppWindowGLFW::SetSourceImage(int width, int height, char* buffer, ColorFormat format)
{
	m_sourceBuffer = buffer;
	m_sourceWidth = width;
	m_sourceHeight = height;
	m_colorFormat = format;

	auto imageFormatInfo = getGLFormat(m_colorFormat);

	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		imageFormatInfo.colorChannel, imageFormatInfo.format, m_sourceBuffer);
}

void AppWindowGLFW::SetEventCallback(const EventCallback & callback)
{
	m_eventCallback = callback;
}

bool AppWindowGLFW::WindowShouldClose()
{
    return glfwWindowShouldClose(m_window);
}


AppWindowGLFW::GLColorFormatResult AppWindowGLFW::getGLFormat(ColorFormat format)
{
	GLColorFormatResult result = { GL_RGB };

	switch (m_colorFormat)
	{
		case ColorFormat::RGBByte: result.format = GL_UNSIGNED_BYTE; break;
		case ColorFormat::RGBFloat: result.format = GL_FLOAT; break;
		default: break;
	}

	return result;
}

void AppWindowGLFW::renderQuad(int textureId)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glUseProgram(m_screenQuadShader);
	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	

}

void AppWindowGLFW::Update()
{

	auto imageFormatInfo = getGLFormat(m_colorFormat);

	/* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, m_textureId);

	glEnable(GL_TEXTURE_2D);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_sourceWidth, m_sourceHeight,
					imageFormatInfo.colorChannel, imageFormatInfo.format, m_sourceBuffer);
	            // GL_RGBA, GL_FLOAT, outputPtr);    
		        // GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());    
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	renderQuad(m_textureId);


    /* Swap front and back buffers */
    // glfwSwapBuffers(m_window);

    /* Poll for and process events */
	glfwPollEvents();
}


void AppWindowGLFW::GetMousePos(float & x, float & y)
{
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    x = xpos;
    y = ypos;
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

    if (action == GLFW_PRESS){
		MousePressedEvent event = {};
		event.mouseCode = button;
		if (m_eventCallback != nullptr) 
		{
			m_eventCallback(event);
		}
	}
	else if (action == GLFW_RELEASE)
	{
		MouseReleaseEvent event = {};
		event.mouseCode = button;
		if (m_eventCallback != nullptr) 
		{
			m_eventCallback(event);
		}
    }


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
	else if (action == GLFW_RELEASE)
	{
		KeyReleaseEvent event = {};
		event.keyCode = key;
		if (m_eventCallback != nullptr) 
		{
			m_eventCallback(event);
		}
	}

}

