#include "ParallelRenderer.h"
#include <ctime>




void ParallelRenderer::StartRender()
{
	std::cout << "Parallel Renderer" << std::endl;
	StartTask();
}


void ParallelRenderer::StartTask() {
	stopFlag = false;
	renderThread = std::thread(&ParallelRenderer::RenderTask, this);
}

void ParallelRenderer::EndTask() {
	stopFlag = true;
	renderThread.join();
}


void ParallelRenderer::RenderTask() {
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
			t[i] = std::thread(&ParallelRenderer::SplitRender, this, 0, 0, width, height, i, threadNum);
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


void ParallelRenderer::SplitRender(int x_start, int y_start, int x_end, int y_end, int offset, int threadNum){
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
