#ifndef RENDER_H
#define RENDER_H
#include "scene.h"
#include "pipeline.h"
class Render {
	std::vector<uPtr<Pipeline>> pipelines;
	uPtr<Scene> scene;
	uPtr<RenderText> renderText;
	Pipeline* AddPipeline(uPtr<Pipeline>& pipe);
	void ConstructPipelines();
	void PrepareRender();
	//called after render each pipeline
	void ResetRenderText();
public:
	Render();
	void SingleRender();
	//interact
	void ChangeFrameBufferSize(int _width, int _height, bool interactWindow);
	Scene* GetScene();
	RenderText* GetRenderText();
	Pipeline* GetPipeline(PIPELINE_NAME name);
};

#endif