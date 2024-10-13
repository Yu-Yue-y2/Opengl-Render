#ifndef MATERIA_H
#define MATERIA_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
enum class MaterialType {
	BASEMATERIAL,
	MATERIALTYPENUM
};
class Material {
	glm::vec3 diffuse = glm::vec3(1.0);
	glm::vec3 specular = glm::vec3(0.3);
	glm::vec3 ambient = glm::vec3(0.1);
	Texture* diffuseMap = nullptr;
	Texture* specularMap = nullptr;
	Texture* normalMap = nullptr;
	MaterialType materialType;
public:
	//set
	void SetDiffuse(glm::vec3 diffuse = glm::vec3(1.0), Texture* diffuseMap = nullptr);
	void SetSpecular(glm::vec3 specular = glm::vec3(1.0), Texture* specularMap = nullptr);
	void SetAmbient(glm::vec3 _ambient = glm::vec3(1.0));
	void SetNormal(Texture* normalMap = nullptr);
	virtual void GetMaterialData(std::vector<float>* data, unsigned int &lenth) const;
	Material(Texture* diffuseMap = nullptr, Texture* specularMap = nullptr, Texture* normalMap = nullptr);
	void GetTexture(std::vector<Texture*>& textures) const;
	const Texture* GetDiffuseMap() const;
	glm::vec3 GetDiffuse();
	glm::vec3 GetSpecular();
	glm::vec3 GetAmbient();
	const Texture* GetSpecularMap() const;
	const Texture* GetNormalMap() const;
	MaterialType GetType() const;
};
#endif // !MATERIA_H
