#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include"math/Vec3.h"
#include"math/Quaternion.h"
#include"Renderer/Camera.h"
#include"Renderer/TestScene1.h"
#include"Renderer/Renderer.h"


#define GL_SILENCE_DEPRECATION
// #include <glad/glad.h>

#include <GLFW/glfw3.h>

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
Scene scene;
Camera cam;

int main(){

	Vec3 v3 = Vec3(1.0f, 2.0f, 3.0f);
	Vec3 v3_2 = Vec3(2.0, 3.0, 10.0f);
	std::cout << "Test Proj";
	std::cout << v3.x << "  " << v3.y << std::endl;

	//v3 += v3_2;
	std::cout << v3.x << "  " << v3.y << std::endl;

    Quaternion q;

    std::cout << "Dot " << Vec3::Dot(v3, v3_2) << std::endl;
    std::cout << "Angle" << Vec3::Angle(v3, v3_2) << std::endl;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    scene = make_test_scene1();

    int width = 640;
    int height = 480;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

	std::cout << "window";
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSwapInterval(1);

    cam = Camera(width, height, Vec3(0.0f, 0.0f, 0.0f));
    // cam.FullRender();
    // cam.RenderScene(&scene);
    std::cout << "cam init done";
    Renderer renderer = Renderer();
    renderer.SetRenderData(&scene, &cam);
    renderer.StartRender();


	unsigned int textureId;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, 640, 0, 480, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    std::cout << "begin render" << std::endl; 
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


    glBindTexture(GL_TEXTURE_2D, textureId);	

    glEnable(GL_TEXTURE_2D);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());    
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0, 0);
    glTexCoord2i(0, 1); glVertex2i(0, height);
    glTexCoord2i(1, 1); glVertex2i(width, height);
    glTexCoord2i(1, 0); glVertex2i(width, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


    }

    glfwTerminate();

	return 0;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if(action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "-----------------" << std::endl;
        std::cout << std::endl << "x " << xpos << ". y " << ypos << std::endl;
        //cam.RenderScene(&scene);
        material * m = scene.materials[1];
        m->color = m->color * 0.2;
        
        // cam.RenderScenePixel(&scene, xpos, ypos);
        // cam.TryRayCast(&scene, {5, 0.321, -19.0747}, {-0.2535, 0.01632, -0.9671});
        std::cout << "-----------------" << std::endl;

    }
}

