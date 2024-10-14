#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "light.h"
#include "vertex_data.h"
#include "material.h"
enum ModelType {
	RENDER,
	SKYBOX,
	POST
};
class Model {
	std::vector<float> data;
	uPtr<VertexArray> vao = nullptr;
public:
	bool has_normal = false;
	bool has_uv = false;
	bool has_tbn = false;
	Model(const char* path, bool caltbn = false);
	Model() {};

	void DirectlyLoadData(std::vector<float>& _data);
	uPtr<VerData> LoadData();
	void BindVaoData();
	void ClearData();
	void SetVaoData();
	void Render() const;
};
class Instance
{
	Model& model;
	ModelType modelType;
	glm::mat4 model_transform;
	glm::mat4 normal_transform;
	Material material;
	const Light* followLight = nullptr;
public:
	const char* name = nullptr;
	Instance(Model& _model, glm::mat4 _model_trans, const char* _name, Material mat = Material(), ModelType modelt = RENDER);
	//Instance(const Instance& instance);
	ModelType GetModelType() const;
	const Model& GetModel() const;
	glm::mat4 GetModelMatrix() const;
	glm::mat4 GetNormalMatrix() const;
	void SetMaterial(Material material);
	Material GetMaterial() const;
	void SetFollowLight(const Light* light);
	const Light* GetLigtht() const;
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3);
	void SetModelTransform(glm::mat4 m);
};
#endif