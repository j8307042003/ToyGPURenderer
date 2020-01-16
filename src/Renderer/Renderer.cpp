#include "Renderer.h"
#include <ctime>

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
	const int threadNum = 4;
	const int width = cam->GetWidth();
	const int height = cam->GetHeight();

	const int testRenderCount = 3;
	itNum = 1;
	int widthDiff = (width / threadNum);

	std::clock_t start;
    double duration;

    start = std::clock();
    int renderLoop = 0;
	while(true) {
		if (stopFlag) return;
        // cam->RenderScene(s);

		std::thread t[threadNum];
		for(int i = 0 ; i < threadNum; i++) {
			//t[i] = std::thread(&Renderer::SplitRender, this, widthDiff * i, 0, (widthDiff) * (i + 1) - 1, height);
			t[i] = std::thread(&Renderer::SplitRender, this, 0, 0, width, height, i, threadNum);
		}

		for (int i = 0 ; i < threadNum; i++){
			t[i].join();
		}

		if (itNum == testRenderCount) {
    		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    		std::cout<<"render Time  "<< duration <<'\n';			
		}

        std::this_thread::sleep_for (std::chrono::milliseconds(100));
		itNum++;
	}
}


void Renderer::SplitRender(int x_start, int y_start, int x_end, int y_end, int offset, int threadNum){
	struct TraceRayBundle;
	struct TraceRay {
		Ray ray;
		TraceRayBundle * bundle;
	};
	struct TraceRayBundle
	{
		TraceRay traceRay[64];
		TraceRayBundle * parent;
		material * m;
		Vec3 colorSum;
		int depth;
		bool doneRequestRay;
	};

	const int maxStack = 512;
	void* bundleStack = malloc(sizeof(TraceRayBundle) * maxStack);

	srand(time(NULL));
	for(int x = x_start + offset; x <= x_end; x += threadNum){
		for(int y = y_start; y < y_end; y++){
			cam->RenderScenePixel(s, x, y, bundleStack, itNum);
		}
	}

	free(bundleStack);
}
