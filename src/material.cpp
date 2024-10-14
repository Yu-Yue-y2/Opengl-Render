#include "material.h"
void Material::GetMaterialData(std::vector<float>* data, unsigned int &lenth) const
{
	lenth = 12;//12 * sizeof(float)
	if (data == nullptr)
	{
		return;
	}
	//std 140
	data->reserve(data->size() + lenth);
	PUSH_VEC3_TO_P_VECTOR(diffuse, data);
	data->push_back(float(materialType));
	PUSH_VEC3_TO_P_VECTOR(specular, data);
	data->push_back(0.f);
	PUSH_VEC3_TO_P_VECTOR(ambient, data);
	data->push_back(0.f);
}
void Material::SetDiffuse(glm::vec3 _diffuse, Texture* _diffuseMap)
{
	diffuse = _diffuse;
	if (_diffuseMap)
		diffuseMap = _diffuseMap;
}
void Material::SetSpecular(glm::vec3 _specular, Texture* _specularMap)
{
	specular = _specular;
	if (_specularMap)
		specularMap = _specularMap;
}
void Material::SetAmbient(glm::vec3 _ambient)
{
	ambient = _ambient;
}
void Material::SetNormal(Texture* _normalMap)
{
	normalMap = _normalMap;
}
Material::Material(Texture* _diffuseMap, Texture* _specularMap, Texture* _normalMap)
	:diffuseMap (_diffuseMap), specularMap(_specularMap), normalMap(_normalMap), materialType(MaterialType::BASEMATERIAL) {}

void Material::GetTexture(std::vector<Texture*>& textures) const
{
	if (diffuseMap)
		textures.push_back(diffuseMap);
	if (specularMap)
		textures.push_back(specularMap);
	if (normalMap)
		textures.push_back(normalMap);
}
const Texture* Material::GetDiffuseMap() const
{
	return diffuseMap;
}
const Texture* Material::GetSpecularMap() const
{
	return specularMap;
}
const Texture* Material::GetNormalMap() const
{
	return normalMap;
}
MaterialType Material::GetType() const
{
	return materialType;
}
glm::vec3 Material::GetDiffuse()
{
	return diffuse;
}
glm::vec3 Material::GetSpecular()
{
	return specular;
}
glm::vec3 Material::GetAmbient()
{
	return ambient;
}