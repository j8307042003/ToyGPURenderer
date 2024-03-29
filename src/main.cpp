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
#include <glm/gtx/quaternion.hpp>

#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
Scene scene;
Camera cam;
Renderer * renderer;

bool keys[1024];
bool mouse_keys[64] = {};

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void Do_Movement();
void Do_Rotate();

double mouse_pos_x = 0, mouse_pos_y = 0;

GLFWwindow * window = nullptr;

float rot_x = 0;
float rot_y = 0;

int main(){


    // GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // scene = make_test_scene1();
    scene = make_test_scene2();
    scene.BuildTree();



    int width = 1280; //640;
    int height = 960; //480;

    // width = 320;
    // height = 480;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Renderer", NULL, NULL);

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

    cam = Camera(width, height, Vec3(200.0f, 0.0f, -200.0f));
    //std::cout << "cam init done";
    // Renderer * renderer = new ParallelRenderer();
    renderer = new VulkanRenderer();
    renderer->SetRenderData(&scene, &cam);
    renderer->StartRender();

    /*
    cam.transform.position = {0, 0.74, -25.8};
    rot_y = 5.164;
    rot_x = 180;
    cam.transform.rotation = 
        glm::normalize(glm::quat(glm::radians(glm::vec3{rot_y, -rot_x, 180 })));    
    */
    // cam.transform.position = {1.427+2, 0.244, -30.5};
    cam.transform.position = {-0.671992, 0.378616, -30.7013}; // dragon
    // rot_y = 12.3; rot_x = 100;
    rot_y = 16.3185; rot_x = -127.658; // dragon
    cam.transform.rotation = 
        glm::normalize(glm::quat(glm::radians(glm::vec3{rot_y, -rot_x, 180 })));     
    cam.transform.UpdateMatrix();    

	unsigned int textureId;
	glGenTextures(1, &textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0,
             GL_RGB, GL_FLOAT, cam.GetBuffer());
             // GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    std::cout << "begin render" << std::endl; 

   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        Do_Rotate();
		Do_Movement();
        // std::cout << "update frame" << std::endl;
        renderer->UpdateFrame();



        //stall gpu renderer. clean up gpu usage for image display
        renderer->Stall();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


		glBindTexture(GL_TEXTURE_2D, textureId);	

		glEnable(GL_TEXTURE_2D);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    GL_RGB, GL_FLOAT, cam.GetBuffer());    
                    // GL_RGBA, GL_FLOAT, outputPtr);    
			        // GL_RGBA, GL_UNSIGNED_BYTE, cam.GetBuffer());    
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

        renderer->Resume();
    }

    glfwTerminate();

	return 0;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    //if(action == GLFW_PRESS) {
    //    double xpos, ypos;
    //    glfwGetCursorPos(window, &xpos, &ypos);
    //    std::cout << "-----------------" << std::endl;
    //    std::cout << std::endl << "x " << xpos << ". y " << ypos << std::endl;
    //    renderer->ClearImage();
    //    std::cout << "-----------------" << std::endl;
    //}

    if (action == GLFW_PRESS){
        glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
        mouse_keys[button] = true;
    }
    else if (action == GLFW_RELEASE)
        mouse_keys[button] = false;

}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
	float x = 0, y = 0, z = 0;

	if (keys[GLFW_KEY_W]) {
		z += -1;
	}
	if (keys[GLFW_KEY_S]) {
		z += 1;
	}
	if (keys[GLFW_KEY_A]) {
		x += -1;
	}
	if (keys[GLFW_KEY_D]) {
		x += 1;
	}
	if (keys[GLFW_KEY_E]) {
		y += 1;
	}
	if (keys[GLFW_KEY_Q]) {
		y += -1;
	}

	if (x != 0 || y != 0 || z != 0) {
        Vec3 dir = cam.transform.TransformDir({x,y,z});
		cam.transform.position += dir * deltaTime * 4.0f;
        cam.transform.UpdateMatrix();
		renderer->ClearImage();
        std::cout << "position : " << cam.transform.position.tostring() << std::endl;

	}

}


void Do_Rotate()
{
    if (mouse_keys[GLFW_MOUSE_BUTTON_1]) {
        double now_pos_x, now_pos_y;
        glfwGetCursorPos(window, &now_pos_x, &now_pos_y);

        float delta_x = now_pos_x - mouse_pos_x;
        float delta_y = now_pos_y - mouse_pos_y;
        if( abs(delta_x) < 5 && abs(delta_y) < 5 )
            return;

        mouse_pos_x = now_pos_x;
        mouse_pos_y = now_pos_y;

        const float kRotRatio = 1 / 10.0f;

        rot_x += delta_x * kRotRatio;
        rot_y += delta_y * kRotRatio;

        // const float kRotRatio = 1 / 10.0f;
        cam.transform.rotation = 
            // glm::normalize(cam.transform.rotation * glm::quat( delta_y * kRotRatio, delta_x * kRotRatio, 0, 0 ));
            glm::normalize(glm::quat(glm::radians(glm::vec3{rot_y, -rot_x, 180 })));
            // glm::normalize(cam.transform.rotation * glm::quat({delta_y * kRotRatio, delta_x * kRotRatio, 0 }));
        cam.transform.UpdateMatrix();
        renderer->ClearImage();

        std::cout << "rotation : " << rot_y << "  " << rot_x << std::endl;

    }
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}



