#ifndef PIPELINE_H
#define PIPELINE_H
#include "renderText.h"
#include "shader.h"
#include "scene.h"
enum class PIPELINE_NAME {
	SHADOW,
	MAIN,
	VISIBLE,
	SKYBOX,
	POST,
	PIPELINENUM
};

enum UniformId {
	MATRICES,
	LIGHTS,
	MATERIALS,
	UNIFORMNUM
};

class Pipeline {
protected:
	uPtr<ShaderProgram> shader;
	std::vector<uPtr<UniformBlock>> uniforms;
	sPtr<FrameBuffer> fbo;
	std::vector<Instance*> instances;
	std::vector<Texture*> free_textures;
	void SetFreeTextureUniform(RenderText* renderText) const;
	void DrawInstanceVao(unsigned int InsatnceId) const;
	void StartRender();
	void EndRender();
public:
	const PIPELINE_NAME pipeline_name;
	//get object
	sPtr<FrameBuffer> GetFrameBuffer() const;

	void BindFBO();
	void AddInstances(const std::vector<uPtr<Instance>>& instances);
	Pipeline(const Scene* scene, PIPELINE_NAME name);
	Pipeline() = delete;

	//glClear and control opengl functions
	virtual void SetRenderConfig() = 0;
	virtual void ClearRenderConfig() = 0;
	//put value in the uniform var in shader
	virtual void SetGlobalUniformValue(RenderText* renderText, Scene* scene) = 0;
	virtual void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene) = 0;
	//circulate all instance and render
	void Render(RenderText* renderText, Scene* scene);
	//interact
	virtual void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText) = 0;
};

//calculate shadow map
class ShadowPipeline : public Pipeline{
public:
	ShadowPipeline(const RenderText* renderText, Scene* scene);
	ShadowPipeline() = delete;
private:
	void SetRenderConfig();
	void ClearRenderConfig();
	void SetGlobalUniformValue(RenderText* renderText, Scene* scene);
	void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene);
public:
	//render context
	void SetRenderTextShadowInf(ShadowInf& shadowInf);
	void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText);
};

class MainPipeline : public Pipeline {
public:
	MainPipeline(const RenderText* renderText, Scene* scene);
	MainPipeline() = delete;
	//unique
	void GetInstances(std::vector<Instance*>& i_nstances);
private:
	void SetRenderConfig();
	void ClearRenderConfig();
	void SetGlobalUniformValue(RenderText* renderText, Scene* scene);
	void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene);
	void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText);
	//unique
	unsigned int GetAllInstanceMaterialSize() const;
	
};

//in geo render normal or other things
class DebugPipeline : public Pipeline {
public:
	DebugPipeline(const RenderText* renderText, Scene* scene);
	DebugPipeline() = delete;
private:
	void SetRenderConfig();
	void ClearRenderConfig();
	void SetGlobalUniformValue(RenderText* renderText, Scene* scene);
	void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene);
	void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText);
};
class SkyboxPipeline : public Pipeline {
public:
	SkyboxPipeline(const RenderText* renderText, Scene* scene);
	SkyboxPipeline() = delete;
	void ChangeSkyboxSize(float);
private:
	void SetRenderConfig();
	void ClearRenderConfig();
	void SetGlobalUniformValue(RenderText* renderText, Scene* scene);
	void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene);
	void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText);
	
};
class PostPipeline : public Pipeline {
public:
	PostPipeline(const RenderText* renderText, Scene* scene);
	PostPipeline() = delete;
private:
	void SetRenderConfig();
	void ClearRenderConfig();
	void SetGlobalUniformValue(RenderText* renderText, Scene* scene);
	void SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene);
	void ChangeFrameBufferSize(int _width, int _height, RenderText* renderText);
};
#endif // !PIPELINE_H

