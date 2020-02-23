#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include"math/Vec3.h"
#include"math/Quaternion.h"
#include"Renderer/Camera.h"
#include"Renderer/TestScene1.h"
#include"Renderer/Renderer.h"
#include"Renderer/ParallelRenderer.h"
#include"Renderer/VulkanRenderer.h"

#include<vector>
#include"BVH/BVH.h"
#include"BVH/AABB.h"

#define GL_SILENCE_DEPRECATION
// #include <glad/glad.h>

#include <GLFW/glfw3.h>

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
Scene scene;
Camera cam;
Renderer * renderer;

int main(){

    Quaternion q;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // scene = make_test_scene1();
    scene = make_test_scene2();
    scene.BuildTree();

    std::vector<AABB> boxes;
    boxes.push_back(AABB({0,0,0}, {1,1,1}));
    boxes.push_back(AABB({-1,-1,-1}, {2,2,2}));
    boxes.push_back(AABB({3,3,3}, {4,4,4}));
    boxes.push_back(AABB({5,5,5}, {5,5,6}));
    boxes.push_back(AABB({8,8,8}, {9,9,9}));

    bvh_tree tree;
    // build_bvh_simple(tree, boxes);


    int width = 1280; //640;
    int height = 960; //480;

    // width = 320;
    // height = 480;

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
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    cam = Camera(width, height, Vec3(0.0f, 0.0f, -10.0f));
    // cam.FullRender();
    // cam.RenderScene(&scene);
    std::cout << "cam init done";
    // Renderer * renderer = new ParallelRenderer();
    renderer = new VulkanRenderer();
    renderer->SetRenderData(&scene, &cam);
    renderer->StartRender();


	unsigned int textureId;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    std::cout << "begin render" << std::endl; 
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        renderer->UpdateFrame();
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
        /*
        const int maxStack = 512;
        void* bundleStack = malloc(40 * maxStack);

        cam.RenderScenePixel(&scene, xpos, ypos, bundleStack, 1);

        free(bundleStack);
        */
        renderer->ClearImage();
        // cam.RenderScenePixel(&scene, xpos, ypos);
        // cam.TryRayCast(&scene, {5, 0.321, -19.0747}, {-0.2535, 0.01632, -0.9671});
        std::cout << "-----------------" << std::endl;

    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    float x = 0, y = 0, z = 0;
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) {
            z += -1;
        }
        else if (key == GLFW_KEY_S) {
            z += 1;
        }
        else if (key == GLFW_KEY_A) {
            x += -1;
        }    
        else if (key == GLFW_KEY_D) {
            x += 1;
        }
        else if (key == GLFW_KEY_E) {
            y += 1;
        }
        else if (key == GLFW_KEY_Q) {
            y += -1;
        }  

        if (x != 0 || y != 0 || z != 0 ) {
            cam.transform.position += Vec3(x, y, z);
            renderer->ClearImage();
        }                          
    }

}



