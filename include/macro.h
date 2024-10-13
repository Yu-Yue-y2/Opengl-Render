#ifndef MACRO_H
#define MACRO_H
#include <string>
#include<memory>
#define DEBUG(id,aim,func) {int  success;\
char infoLog[1024];\
func(id, aim, &success);\
if (!success)\
{\
	glGetShaderInfoLog(id, 1024, NULL, infoLog);\
	std::cout << "ERROR:: "<<#func <<" "<<#aim << infoLog << std::endl;\
	return;\
}}

#define mkU std::make_unique
#define uPtr std::unique_ptr
#define mkS std::make_shared
#define sPtr std::shared_ptr
#define LAYOUT_(x) (x)


#define VERTEX_SHADOW_SHADER_PATH ("./shader/shadow.vs")
#define FRAGMENT_SHADOW_SHADER_PATH ("./shader/shadow.fs")
#define VERTEX_MAIN_SHADER_PATH ("./shader/test.vs")
#define FRAGMENT_MAIN_SHADER_PATH ("./shader/test.fs")
#define VERTEX_POST_SHADER_PATH ("./shader/post.vs")
#define FRAGMENT_POST_SHADER_PATH ("./shader/post.fs")
#define VERTEX_SKYBOX_SHADER_PATH ("./shader/skybox.vs")
#define FRAGMENT_SKYBOX_SHADER_PATH ("./shader/skybox.fs")
#define VERTEX_VISBLE_SHADER_PATH ("./shader/visible.vs")
#define FRAGMENT_VISBLE_SHADER_PATH ("./shader/visible.fs")
#define GEOMETR_VISBLE_SHADER_PATH ("./shader/visible.gs")

#define TEXTURE_PATH std::string("./assets/texture/")
#define TEXTURE_PATH_1 ("./assets/texture/container.jpg")
#define TEXTURE_PATH_2 ("./assets/texture/wall.jpg")
#define TEXTURE_PATH_DIFFUSE ("./assets/texture/box_diffuse.png")
#define TEXTURE_PATH_SPECULAR ("./assets/texture/box_specular.png")
#define TEXTURE_PATH_BRICKWALL (TEXTURE_PATH + std::string("brickwall.jpg"))
#define TEXTURE_PATH_BRICKWALL_NORAML (TEXTURE_PATH + std::string("brickwall_normal.jpg"))


#define SKYBOXPATH std::string("./assets/texture/skybox/")
#define SKYBOXBACK (SKYBOXPATH + std::string("back.jpg"))
#define SKYBOXBOTTOM (SKYBOXPATH + std::string("bottom.jpg"))
#define SKYBOXFRONT (SKYBOXPATH + std::string("front.jpg"))
#define SKYBOXLEFT (SKYBOXPATH + std::string("left.jpg"))
#define SKYBOXRIGHT (SKYBOXPATH + std::string("right.jpg"))
#define SKYBOXTOP (SKYBOXPATH + std::string("top.jpg"))


#define MODEL_PATH std::string("./assets/obj/")

#define STR(x) #x

//texture uniform name
#define DIFFUSE_TEXTURE_NAME ("diffuse_texture")
#define NORMAL_TEXTURE_MAPNAME ("normal_map")
#define SKYBOXNAME ("skybox_texture")


#define PUSH_VEC3_TO_P_VECTOR(VEC, VECTOR)  {\
for(int i = 0;i < 3; i++)\
	VECTOR->push_back(VEC[i]);\
}
#define PRINT_ERROR() { int errorCode;\
	while ((errorCode = glGetError()) != GL_NO_ERROR) { \
		std::cout << "[OpenGL_Error(" << errorCode << ")]" << std::endl; \
		break; \
	} \
}
#endif