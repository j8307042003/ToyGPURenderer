#include "Renderer.h"


class ParallelRenderer : public Renderer {
public:
	void StartRender();
	void UpdateFrame() {}
	void ClearImage() {}
private:
	bool stopFlag;
	std::thread renderThread;
	int itNum = 0;
	void StartTask();
	void EndTask();

	void RenderTask();
	void SplitRender(int x_start, int y_start, int x_end, int y_end,  int offset, int threadNum);

};