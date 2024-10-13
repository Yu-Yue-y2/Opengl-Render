#include"scene.h"
Scene::Scene(RenderText* renderText)
{
	//light
	uPtr<Light> pl = mkU<PointLight>();
	pl->intensity = glm::vec3(1.0);
	(dynamic_cast<PointLight*>(pl.get()))->pos = (glm::vec3(0.0, 1.0, 1.0));

	uPtr<Light> dl = mkU<DirectionLight>();
	dl->intensity = glm::vec3(1.0);
	(dynamic_cast<DirectionLight*>(dl.get()))->dir = glm::normalize(glm::vec3(0.0, -1.0, 1.0));
	Light* light = AddLight(dl, true);

	//main_camera
	uPtr<Camera> camera = mkU<Camera>();
	SetCamera(camera);
	
	//texture + model
	uPtr<Texture> _t1 = mkU<Texture>();
	_t1->SetName(DIFFUSE_TEXTURE_NAME);
	_t1->Load((MODEL_PATH + std::string("africanhead/african_head_diffuse.tga")).c_str());
	Texture* AfricanHeadDiffuseTexture = AddTexture(_t1);

	uPtr<Texture> _grid_texture = mkU<Texture>(TEXTURE_PATH_BRICKWALL.c_str(), DIFFUSE_TEXTURE_NAME);
	uPtr<Texture> _grid_normal_texture = mkU<Texture>(TEXTURE_PATH_BRICKWALL_NORAML.c_str(), NORMAL_TEXTURE_MAPNAME, false, true);
	Texture* grid_texture = AddTexture(_grid_texture);
	Texture* grid_normal_texture = AddTexture(_grid_normal_texture);

	std::vector<std::string> skybox_paths = { SKYBOXRIGHT, SKYBOXLEFT, SKYBOXTOP,SKYBOXBOTTOM,SKYBOXFRONT,SKYBOXBACK };
	uPtr<Texture> _texture_skybox = mkU<CubeTexture>(skybox_paths);
	_texture_skybox->SetName(SKYBOXNAME);
	Texture* texture_skybox = AddTexture(_texture_skybox);
	renderText->sharedObject.SkyboxTextureId = GetTextureNum() - 1;


	uPtr<Model> u_model1 = mkU<Model>((MODEL_PATH + std::string("africanhead/african_head.obj")).c_str(), false);
	Model* model1 = AddModel(u_model1);
	Material AfricanHeadMaterial(AfricanHeadDiffuseTexture);
	Instance AfricanHeadInstance(*model1, glm::rotate(glm::mat4(), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)), AfricanHeadMaterial);
	AddInstance(AfricanHeadInstance);

	uPtr<Model> u_plane_tbn = mkU<Model>((MODEL_PATH + std::string("cube/cube.obj")).c_str(), true);
	Material gridPlaneMaterial(grid_texture, nullptr, grid_normal_texture);


	Model* plane_tbn = AddModel(u_plane_tbn);
	AddInstance(Instance(*plane_tbn,
		glm::translate(glm::mat4(), glm::vec3(0.0, -1.0, 0.0)) * glm::scale(glm::mat4(), glm::vec3(15.0, 0.2, 15.0)),
		gridPlaneMaterial));
	AddInstance(Instance(*plane_tbn,
		glm::translate(glm::mat4(), glm::vec3(30.0, -1.0, 0.0)) * glm::scale(glm::mat4(), glm::vec3(15.0, 0.2, 15.0)),
		gridPlaneMaterial));
	AddInstance(Instance(*plane_tbn,
		glm::translate(glm::mat4(), glm::vec3(0.0, 14.01, -15)) * glm::scale(glm::mat4(), glm::vec3(15.0, 15.0, 0.5)),
		gridPlaneMaterial));
	AddInstance(Instance(*plane_tbn,
		glm::translate(glm::mat4(), glm::vec3(30.0, 14.01, -15)) * glm::scale(glm::mat4(), glm::vec3(15.0, 15.0, 0.5)),
		gridPlaneMaterial));
	AddInstance(Instance(*plane_tbn,
		glm::translate(glm::mat4(), glm::vec3(40.5, 14.05, 0.0)) * glm::scale(glm::mat4(), glm::vec3(0.3, 15.0, 15.0)),
		gridPlaneMaterial));

	uPtr<Model> u_lightcube = mkU<Model>((MODEL_PATH + std::string("cube/cube.obj")).c_str());
	Model* lightcube = AddModel(u_lightcube);
	Instance lightCubeInstance(*lightcube,
		glm::translate(glm::mat4(), light->getPos()) * glm::scale(glm::mat4(), glm::vec3(0.6, 0.6, 0.6)));
	lightCubeInstance.SetFollowLight(light);
	AddInstance(lightCubeInstance);

	/*
	uPtr<Model> u_model_bench = mkU<Model>((MODEL_PATH + std::string("bench/Bench_HighRes.obj")).c_str());
	Model* model_bench = AddModel(u_model_bench);
	AddInstance(Instance(*model_bench,
		glm::translate(glm::mat4(), glm::vec3(2.0, -1.0, 2.0)) * glm::scale(glm::mat4(), glm::vec3(0.02, 0.02, 0.02))));
	*/
	uPtr<Model> u_model_skybox = mkU<Model>();
	u_model_skybox->DirectlyLoadData(skyboxVertices);
	Model* model_skybox = AddModel(u_model_skybox);
	AddInstance(Instance(*model_skybox, glm::mat4(), Material(), ModelType::SKYBOX));
	
	uPtr<Model> u_model_post = mkU<Model>((MODEL_PATH + std::string("quad/quad.obj")).c_str());
	Model* model_post = AddModel(u_model_post);
	AddInstance(Instance(*model_post, glm::mat4(), Material(), ModelType::POST));

	SetVao();
	ClearModelData();
}
Model* Scene::AddModel(uPtr<Model>& u_model)
{
	Model& model = *u_model;
	models.push_back(std::move(u_model));
	return models.back().get();
}
Texture* Scene::AddTexture(uPtr<Texture>& u_texture)
{
	Texture& texture = *u_texture;
	textures.push_back(std::move(u_texture));
	return textures.back().get();
}
Light* Scene::AddLight(uPtr<Light>& u_light, bool isMain)
{
	if (isMain)
	{
		mainLight = lights.size();
	}
	Light& light = *u_light;
	lights.push_back(std::move(u_light));
	return lights.back().get();
}
Camera* Scene::SetCamera(uPtr<Camera>& u_camera)
{
	Camera& _camera = *u_camera;
	camera = std::move(u_camera);
	return camera.get();
}
void Scene::AddInstance(uPtr<Instance>& instance)
{
	instances.push_back(std::move(instance));
}
void Scene::AddInstance(const Instance& instance)
{
	uPtr<Instance> u_instance = mkU<Instance>(instance);
	instances.push_back(std::move(u_instance));
}
void Scene::SetVao()
{
	for (uPtr<Model>& model : models)
	{
		model->SetVaoData();
	}
}
const std::vector<uPtr<Instance>>& Scene::GetInstances() const
{
	return instances;
}
void Scene::ClearModelData()
{
	for (uPtr<Model>& model : models)
	{
		model->ClearData();
	}
}
glm::mat4 Scene::GetMainLightVP() const
{
	return lights[mainLight]->getProjection() * lights[mainLight]->getView();
}
Camera* Scene::GetCamera() const
{
	return camera.get();
}
Light* Scene::GetMainLight() const
{
	return lights[mainLight].get();
}
int Scene::GetTextureNum() const
{
	return textures.size();
}
Texture* Scene::GetTexure(unsigned int id) const
{
	return textures[id].get();
}