#version 330 core
in vec3 Textcoor;
out vec4 FragColor;
uniform samplerCube skybox_texture;
#define CUBEMAP_SKYBOX 0
uniform int skybox_type;
void main()
{             
	if(skybox_type == CUBEMAP_SKYBOX)
	{
		FragColor = texture(skybox_texture, Textcoor);
	}
}