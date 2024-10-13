#ifndef RENDER_TEXT_H
#define RENDER_TEXT_H
#include "vertex_data.h"
#include "texture.h"
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
enum class SHADOWTYPE {
	NONE,
	NORMAL,
	PCF,
	PCSS
};
struct ShadowInf {
	SHADOWTYPE shadow_type = SHADOWTYPE::NONE;
	int core_half;
	float biascontrol;
	float biasoffset;
	sPtr<FrameBuffer> shadowMap;
};
enum class HdrType {
	HDR_NONE,
	HDR_REINHARD,
	HDR_EXP,
	HDR_FILMIC,
	HDR_ACES
};
struct HdrInf {
	HdrType hdr_type = HdrType::HDR_NONE;
	float exposure = 1.0;
};
struct GeometryInf 
{
	int extra_line = 1;
};

struct PipelineInf 
{
	bool has_geometryTest = false;
};
struct SharedObject {
	unsigned int SkyboxTextureId = -1;
	sPtr<FrameBuffer> MainFbo = nullptr;
};
struct CalculationResult {
	glm::mat4 light_vp;
	glm::mat4 camera_view;
	glm::mat4 camera_projection;
	glm::mat4 camera_vp;
};
struct RenderText {
	//used for render record calculation result
	unsigned int textureDynamicCount = 0;
	CalculationResult calculationRe;
	SharedObject sharedObject;
	ShadowInf shadowInf;
	HdrInf hdrInf;
	GeometryInf geoInf;
	PipelineInf pipeInf;
};
#endif