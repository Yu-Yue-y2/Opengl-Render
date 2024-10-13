#include "pipeline.h"
#include "window.h"
Pipeline::Pipeline(const Scene* scene, PIPELINE_NAME name)
	:pipeline_name(name)
{
	AddInstances(scene->GetInstances());
}
void Pipeline::SetFreeTextureUniform(RenderText* renderText) const
{
	for (auto texture : free_textures)
	{
		shader->setTexture(renderText->textureDynamicCount, texture);
		renderText->textureDynamicCount++;
	}
}
void Pipeline::DrawInstanceVao(unsigned int InstanceId) const
{
	const Instance* instance = instances[InstanceId];
	const Model& model = instance->GetModel();
	model.Render();
}
void Pipeline::StartRender()
{
	shader->Active();
	BindFBO();
	
}
void Pipeline::EndRender()
{
	if (fbo)
		fbo->DisActive();
	shader->DisActive();
}
void Pipeline::Render(RenderText* renderText, Scene* scene)
{
	StartRender();
	SetRenderConfig();
	SetGlobalUniformValue(renderText, scene);
	for (int InstanceId = 0; InstanceId < instances.size(); InstanceId++)
	{
		SetInstanceUniformValue(renderText, InstanceId, scene);
		DrawInstanceVao(InstanceId);
	}
	ClearRenderConfig();
	EndRender();
}
sPtr<FrameBuffer> Pipeline::GetFrameBuffer() const
{
	return fbo;
}
void Pipeline::BindFBO()
{
	if (fbo)
		fbo->Active();
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Pipeline::AddInstances(const std::vector<uPtr<Instance>>& _instances)
{
	for (unsigned int i = 0; i < _instances.size(); i++)
	{
		Instance* instance = _instances[i].get();
		if (pipeline_name == PIPELINE_NAME::MAIN)
		{
			if (instance->GetModelType() == ModelType::RENDER)
			{
				instances.push_back(instance);
			}
		}
		else if (pipeline_name == PIPELINE_NAME::SHADOW)
		{
			if (instance->GetModelType() == ModelType::RENDER && !instance->GetLigtht())
			{
				instances.push_back(instance);
			}
		}
		else if (pipeline_name == PIPELINE_NAME::POST)
		{
			if (instance->GetModelType() == ModelType::POST)
			{
				instances.push_back(instance);
			}
		}
		else if (pipeline_name == PIPELINE_NAME::SKYBOX)
		{
			if (instance->GetModelType() == ModelType::SKYBOX)
			{
				instances.push_back(instance);
			}
		}
	}
}
//shadowPipe
ShadowPipeline::ShadowPipeline(const RenderText* renderText, Scene* scene)
	:Pipeline(scene, PIPELINE_NAME::SHADOW)
{
	uPtr<Shader> vs_shadow = mkU<Shader>(GL_VERTEX_SHADER, VERTEX_SHADOW_SHADER_PATH);
	uPtr<Shader> fs_shadow = mkU<Shader>(GL_FRAGMENT_SHADER, FRAGMENT_SHADOW_SHADER_PATH);
	std::vector<Shader*> shaders_shadow = { vs_shadow.get(), fs_shadow.get() };
	shader = mkU<ShaderProgram>(shaders_shadow);

	fbo = mkS<FrameBuffer>(shadow_map_width, shadow_map_height, true);
}
void ShadowPipeline::SetRenderConfig()
{
	glViewport(0, 0, shadow_map_width, shadow_map_height);
	glCullFace(GL_FRONT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void ShadowPipeline::ClearRenderConfig()
{
	glCullFace(GL_BACK);
}
void ShadowPipeline::SetGlobalUniformValue(RenderText* renderText, Scene* scene)
{
	SetFreeTextureUniform(renderText);
}
void ShadowPipeline::SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene)
{
	const Instance* instance = instances[InstanceId];
	const Model& model = instance->GetModel();
	glm::mat4 m = instance->GetModelMatrix();
	shader->setMatrix4("mvp", (renderText->calculationRe.light_vp * m));
}
void ShadowPipeline::SetRenderTextShadowInf(ShadowInf& shadowInf)
{
	shadowInf.shadow_type = SHADOWTYPE::NONE;
	shadowInf.core_half = 2;
	shadowInf.biascontrol = 1.0;
	shadowInf.biasoffset = 0.003;
	if (fbo)
	{
		shadowInf.shadowMap = fbo;
	}
}
//only depends on shadow_width and shadow_height
void ShadowPipeline::ChangeFrameBufferSize(int _width, int _height, RenderText* renderText) 
{
	fbo->Resize(_width, _height);
}


//mainPipe
MainPipeline::MainPipeline(const RenderText* renderText, Scene* scene)
	:Pipeline(scene, PIPELINE_NAME::MAIN)
{
	uniforms.resize(int(UniformId::UNIFORMNUM));
	uPtr<Shader> vs = mkU<Shader>(GL_VERTEX_SHADER, VERTEX_MAIN_SHADER_PATH);
	uPtr<Shader> fs = mkU<Shader>(GL_FRAGMENT_SHADER, FRAGMENT_MAIN_SHADER_PATH);
	std::vector<Shader*> shaders = { vs.get(), fs.get() };
	shader = mkU<ShaderProgram>(shaders);

	uPtr<UniformBlock> matrices = mkU<UniformBlock>();
	matrices->SetSize(sizeof(glm::mat4) * 4);
	matrices->SetName("Matrices");
	matrices->Bind();
	shader->BindUniformBlock(matrices.get());
	uniforms[UniformId::MATRICES] = (std::move(matrices));

	//uniform buffer for instance material
	uPtr<UniformBlock> materialsUniformBuf = mkU<UniformBlock>();
	materialsUniformBuf->SetSize(GetAllInstanceMaterialSize() * sizeof(float));
	materialsUniformBuf->SetName("RawMaterial");
	materialsUniformBuf->Bind();
	shader->BindUniformBlock(materialsUniformBuf.get());
	uniforms[UniformId::MATERIALS] = (std::move(materialsUniformBuf));
	//skybox
	free_textures.push_back(scene->GetTexure(renderText->sharedObject.SkyboxTextureId));

	fbo = mkS<FrameBuffer>(width, height, false, GL_RGB16F);
}
void MainPipeline::SetRenderConfig()
{
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void MainPipeline::ClearRenderConfig() {}
void MainPipeline::SetGlobalUniformValue(RenderText* renderText, Scene* scene)
{
	shader->setVec4("intensity", scene->GetMainLight()->getData().intensity);
	shader->setVec4("config", scene->GetMainLight()->getData().v1);
	shader->setVec3("viewpos", scene->GetCamera()->Position);
	shader->setInt("ShadowType", int(renderText->shadowInf.shadow_type));
	shader->setInt("core", renderText->shadowInf.core_half);
	shader->setFloat("hpixellenth", 30.0 / ((float)shadow_map_width * 2.0));
	shader->setFloat("biascontrol", renderText->shadowInf.biascontrol);
	shader->setFloat("biasoffset", renderText->shadowInf.biasoffset);
	
	if (renderText->shadowInf.shadowMap)
	{
		shader->setTexture(renderText->textureDynamicCount, renderText->shadowInf.shadowMap->depthTexture.get());
		renderText->textureDynamicCount = renderText->textureDynamicCount + 1;
	}
	
	SetFreeTextureUniform(renderText);
}
void MainPipeline::SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene)
{
	const Instance* instance = instances[InstanceId];
	const Model& model = instance->GetModel();
	glm::mat4 m;
	glm::mat4 normal_m;
	const Light* followLight = instance->GetLigtht();
	if (followLight)
	{
		m = glm::translate(glm::mat4(), followLight->getPos()) * glm::scale(glm::mat4(), glm::vec3(0.6, 0.6, 0.6));
	}
	else
	{
		m = instance->GetModelMatrix();
		normal_m = instance->GetNormalMatrix();
	}
	glm::mat4 shadow_mvps = renderText->calculationRe.light_vp * m;
	glm::mat4 mvp = renderText->calculationRe.camera_vp * m;
	UniformBlock* matrices = uniforms[UniformId::MATRICES].get();
	matrices->Active();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mvp));
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(normal_m));
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(shadow_mvps));
	matrices->DisActive();

	Material material = instance->GetMaterial();
	UniformBlock* materialUniformBuf = uniforms[UniformId::MATERIALS].get();
	std::vector<float> materialData;
	unsigned int lenth = 0;
	material.GetMaterialData(&materialData, lenth);
	materialUniformBuf->Active();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * lenth, materialData.data());
	materialUniformBuf->DisActive();
	
	std::vector<Texture*> textures;
	material.GetTexture(textures);
	for (auto texture : textures)
	{
		shader->setTexture(renderText->textureDynamicCount, texture);
		renderText->textureDynamicCount++;
	}
	shader->setBool("hasm_texture", material.GetDiffuseMap() != nullptr);
	shader->setBool("has_tbn", model.has_tbn && material.GetNormalMap() != nullptr);
}
unsigned int MainPipeline::GetAllInstanceMaterialSize() const
{
	unsigned int count = 0;
	for (const Instance* instance : instances)
	{
		Material mat = instance->GetMaterial();
		unsigned int lenth = 0;
		mat.GetMaterialData(nullptr, lenth);
		count += lenth;
	}
	return count;
}
void MainPipeline::ChangeFrameBufferSize(int _width, int _height, RenderText* renderText)
{
	//1th
	// offline render doesn't need change
	// 2th
	//don't need. Resize dosn't change the point of color and depth, it still points to correct place
	//renderText->sharedObject.MainFbo = fbo;
	if (fbo)
		fbo->Resize(_width, _height);
	
}
//DebugPipe
DebugPipeline::DebugPipeline(const RenderText* renderText, Scene* scene)
	:Pipeline(scene, PIPELINE_NAME::VISIBLE)
{
	//shader
	uPtr<Shader> vs_visble = mkU<Shader>(GL_VERTEX_SHADER, VERTEX_VISBLE_SHADER_PATH);
	uPtr<Shader> fs_visble = mkU<Shader>(GL_FRAGMENT_SHADER, FRAGMENT_VISBLE_SHADER_PATH);
	uPtr<Shader> gs_visble = mkU<Shader>(GL_GEOMETRY_SHADER, GEOMETR_VISBLE_SHADER_PATH);
	std::vector<Shader*> shaders_visible = { vs_visble.get(), gs_visble.get(),fs_visble.get() };

	shader = mkU<ShaderProgram>(shaders_visible);
}
void DebugPipeline::SetRenderConfig() {}
void DebugPipeline::ClearRenderConfig()  {}
void DebugPipeline::SetGlobalUniformValue(RenderText* renderText, Scene* scene)
{
	shader->setVec4("config", scene->GetMainLight()->getData().v1);
	shader->setVec3("viewpos", scene->GetCamera()->Position);
	shader->setMatrix4("vp", renderText->calculationRe.camera_vp);
	shader->setInt("emit_num", renderText->geoInf.extra_line);
	SetFreeTextureUniform(renderText);
}
void DebugPipeline::SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene)
{
	const Instance* instance = instances[InstanceId];
	const Model& model = instance->GetModel();
	glm::mat4 m = instance->GetModelMatrix();
	glm::mat4 normal_m = instance->GetNormalMatrix();
	shader->setMatrix4("m", m);
	shader->setMatrix4("normal_m", normal_m);
	shader->setBool("has_tbn", model.has_tbn);
}
//mainpipeline has changed
void DebugPipeline::ChangeFrameBufferSize(int _width, int _height, RenderText* renderText) {}


//Postpipe
PostPipeline::PostPipeline(const RenderText* renderText, Scene* scene)
	:Pipeline(scene, PIPELINE_NAME::POST)
{
	uPtr<Shader> post_vs = mkU<Shader>(GL_VERTEX_SHADER, VERTEX_POST_SHADER_PATH);
	uPtr<Shader> post_fs = mkU<Shader>(GL_FRAGMENT_SHADER, FRAGMENT_POST_SHADER_PATH);
	std::vector<Shader*> shaders_post = { post_vs.get(),post_fs.get() };
	PRINT_ERROR();
	shader = mkU<ShaderProgram>(shaders_post);
	PRINT_ERROR();
	if (renderText->sharedObject.MainFbo)
	{
		free_textures.push_back(renderText->sharedObject.MainFbo->colors[0].get());
	}
}
void PostPipeline::SetRenderConfig()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClear(GL_COLOR_BUFFER_BIT);
}
void PostPipeline::ClearRenderConfig() 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_FRAMEBUFFER_SRGB);
}
void PostPipeline::SetGlobalUniformValue(RenderText* renderText, Scene* scene)
{
	shader->setInt("hdr_type", int(renderText->hdrInf.hdr_type));
	shader->setFloat("exposure", renderText->hdrInf.exposure);
	SetFreeTextureUniform(renderText);
	PRINT_ERROR();
}
void PostPipeline::SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene) {}
void PostPipeline::ChangeFrameBufferSize(int _width, int _height, RenderText* renderText)
{
	if (fbo) 
	{
		fbo->Resize(_width, _height);
	}
}


//Skybox
SkyboxPipeline::SkyboxPipeline(const RenderText* renderText, Scene* scene)
	:Pipeline(scene, PIPELINE_NAME::SKYBOX)
{
	//shader
	uPtr<Shader> vs_visble = mkU<Shader>(GL_VERTEX_SHADER, VERTEX_SKYBOX_SHADER_PATH);
	uPtr<Shader> fs_visble = mkU<Shader>(GL_FRAGMENT_SHADER, FRAGMENT_SKYBOX_SHADER_PATH);
	std::vector<Shader*> shaders_skybox = { vs_visble.get(),fs_visble.get() };

	shader = mkU<ShaderProgram>(shaders_skybox);
	free_textures.push_back(scene->GetTexure(renderText->sharedObject.SkyboxTextureId));
	fbo = renderText->sharedObject.MainFbo;

}
void SkyboxPipeline::SetRenderConfig()
{
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
}
void SkyboxPipeline::ClearRenderConfig() 
{
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}

void SkyboxPipeline::SetGlobalUniformValue(RenderText* renderText, Scene* scene)
{
	shader->setMatrix4("vp", renderText->calculationRe.camera_projection * glm::mat4(glm::mat3(renderText->calculationRe.camera_view)));
	SetFreeTextureUniform(renderText);
}
void SkyboxPipeline::SetInstanceUniformValue(RenderText* renderText, unsigned int InstanceId, Scene* scene) {}
//mainpipeline has changed
void SkyboxPipeline::ChangeFrameBufferSize(int _width, int _height, RenderText* renderText) {}