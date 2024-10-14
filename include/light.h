#ifndef LIGHT_H
#define LIGHT_H
#include "camera.h"
enum LightType {
	DIRECTIONLIGHT,
	POINTLIGHT,
	LIGHTTYPENUM
};
struct LightUniformData {
	glm::vec4 intensity;
	glm::vec4 v1;
};
class Light {
public:
	LightType lighttype;
	glm::vec3 intensity;
	bool computeShadow = false;
	float lenth = 1.0;
	virtual LightUniformData getData() const  = 0;
	virtual glm::mat4 getProjection() const  = 0;
	virtual glm::mat4 getView() const  = 0;
	virtual glm::vec3 getPos() const = 0;
};
class PointLight :public Light {
public:
	glm::vec3 pos;
	PointLight()
	{
		lighttype = POINTLIGHT;
	}
	LightUniformData getData() const
	{
		//glm::vec3 _pos = glm::vec3(vp * glm::vec4(pos, 1.0));
		return { glm::vec4(intensity, computeShadow) , glm::vec4(pos, POINTLIGHT)};
	}
	glm::vec3 getPos() const
	{
		return pos;
	}
	glm::mat4 getProjection() const
	{
		return glm::mat4();
	}
	glm::mat4 getView() const
	{
		return glm::mat4();
	}
};
class DirectionLight :public Light {
public:
	glm::vec3 dir;
	DirectionLight()
	{
		lighttype = DIRECTIONLIGHT;
	}
	LightUniformData getData() const
	{
		//glm::vec3 _dir = glm::vec3(vp * glm::vec4(dir, 0.0));
		return { glm::vec4(intensity, computeShadow) , glm::vec4(dir, DIRECTIONLIGHT) };
		//return { glm::vec4(intensity, lenth) , glm::vec4(dir, DIRECTIONLIGHT)};
	}
	glm::mat4 getProjection() const
	{
		return glm::ortho(-15.0, 15.0, -15.0, 15.0, 0.01, 50.0);
	}
	glm::mat4 getView() const
	{
		return glm::lookAt(-dir * glm::vec3(15.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 getPos() const
	{
		return -dir * glm::vec3(15.0);
	}
};
#endif