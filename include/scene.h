#ifndef SCENE_H
#define SCENE_H
#include "model.h"
#include "light.h"
#include "renderText.h"
class Scene
{
	std::vector<uPtr<Instance>> instances;
	std::vector<uPtr<Model>> models;
	std::vector<uPtr<Texture>> textures;
	std::vector<uPtr<Light>> lights;
	uPtr<Camera> camera = nullptr;
	unsigned int mainLight;
public:
	Model* AddModel(uPtr<Model>& m);
	Texture* AddTexture(uPtr<Texture>& t);
	Light* AddLight(uPtr<Light>& l, bool isMain = false);
	Camera* SetCamera(uPtr<Camera>& camera);
	void AddInstance(uPtr<Instance>& instance);
	void AddInstance(const Instance& instance);
	void SetVao();
	const std::vector<uPtr<Instance>>& GetInstances() const;
	void ClearModelData();
	glm::mat4 GetMainLightVP() const;
	Camera* GetCamera() const;
	Light* GetMainLight() const;
	int GetTextureNum() const;
	Texture* GetTexure(unsigned int id) const;
	int GetLightsSize() const;
	Light* GetLight(int id);
	Scene(RenderText* renderText);
	Scene() = delete;
};
#endif
