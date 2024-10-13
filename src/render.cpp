#include "render.h"
#include "window.h"
Render::Render()
{
	renderText = mkU<RenderText>();
	scene = mkU<Scene>(renderText.get());
	ConstructPipelines();
}
void Render::ConstructPipelines()
{
	uPtr<Pipeline> _pipeline_shadow = mkU<ShadowPipeline>(renderText.get(), scene.get());
	Pipeline* shadow_pipeline = AddPipeline(_pipeline_shadow);

	//shadow inf
	dynamic_cast<ShadowPipeline*>(shadow_pipeline)->SetRenderTextShadowInf(renderText->shadowInf);

	uPtr<Pipeline> _pipeline_main = mkU<MainPipeline>(renderText.get(), scene.get());
	Pipeline* pipeline_main = AddPipeline(_pipeline_main);

	if (pipeline_main->GetFrameBuffer())
		renderText->sharedObject.MainFbo = pipeline_main->GetFrameBuffer();

	if (renderText->pipeInf.has_geometryTest)
	{
		uPtr<Pipeline> pipeline_visible = mkU<DebugPipeline>(renderText.get(), scene.get());
		AddPipeline(pipeline_visible);
	}
	uPtr<Pipeline> pipeline_skybox = mkU<SkyboxPipeline>(renderText.get(), scene.get());
	AddPipeline(pipeline_skybox);
	
	uPtr<Pipeline> pipeline_post = mkU<PostPipeline>(renderText.get(), scene.get());
	AddPipeline(pipeline_post);

	//set global config
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_FRAMEBUFFER_SRGB);
}
void Render::ResetRenderText()
{
	renderText->textureDynamicCount = 0;
}
void Render::PrepareRender()
{
	renderText->textureDynamicCount = 0;
	renderText->calculationRe.light_vp = scene->GetMainLightVP();
	renderText->calculationRe.camera_view = scene->GetCamera()->GetViewMatrix();
	renderText->calculationRe.camera_projection = scene->GetCamera()->GetProjMatrix();
	renderText->calculationRe.camera_vp = renderText->calculationRe.camera_projection * renderText->calculationRe.camera_view;
}
void Render::SingleRender()
{
	PrepareRender();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	for (uPtr<Pipeline>& pipeline : pipelines)
	{
		pipeline->Render(renderText.get(), scene.get());
		ResetRenderText();
	}
}
Pipeline* Render::AddPipeline(uPtr<Pipeline>& pipe)
{
	Pipeline* p = pipe.get();
	pipelines.push_back(std::move(pipe));
	return p;
}

//intweractWindow = true when user change Interactive window, not shadow or offline window
void Render::ChangeFrameBufferSize(int _width, int _height, bool interactWindow)
{
	width = _width;
	height = _height;
	for (uPtr<Pipeline>& pipeline : pipelines)
	{
		if (interactWindow && pipeline->pipeline_name == PIPELINE_NAME::POST)
		{
			pipeline->ChangeFrameBufferSize(_width, _height, renderText.get());
			break;
		}
		else
		{
			pipeline->ChangeFrameBufferSize(_width, _height, renderText.get());
		}
	}
}
Scene* Render::GetScene()
{
	return scene.get();
}
RenderText* Render::GetRenderText()
{
	return renderText.get();
}
