#include "Renderer.h"


void Renderer::SetRenderData(Scene * s, Camera * cam){
	// EndTask();
	this->s = s;
	this->cam = cam;

}

void Renderer::StartRender() {
	//EndTask();
	StartTask();
}

void Renderer::StartTask() {
	stopFlag = false;
	renderThread = std::thread(&Renderer::RenderTask, this);
}

void Renderer::EndTask() {
	stopFlag = true;
	renderThread.join();
}


void Renderer::RenderTask() {

	while(true) {
		if (stopFlag) return;
        cam->RenderScene(s);
        // std::this_thread::sleep_for (std::chrono::milliseconds(100));
        std::this_thread::sleep_for (std::chrono::seconds(5));
	}
}
