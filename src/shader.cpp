#include <glad/glad.h>
#include <iostream>
#include "shader.h"
#include"macro.h"
#include "file.h"

Shader::Shader(unsigned int type, const char* path)
{
	const char* _c = ReadShader(path);
	if ((type != GL_VERTEX_SHADER) && (type != GL_FRAGMENT_SHADER) && (type != GL_GEOMETRY_SHADER))
	{
		std::cout << "error shader type" << std::endl;
		return;
	}
	id = glCreateShader(type);
	//std::cout << _c << std::endl;
	glShaderSource(id, 1, &_c, NULL);
	glCompileShader(id);
	DEBUG(id, GL_COMPILE_STATUS, glGetShaderiv);
	delete _c;
}

Shader::~Shader()
{
	std::cout << "delete shader" << id << std::endl;
	glDeleteShader(id);
}
ShaderProgram::ShaderProgram(std::vector<Shader*>& shaders)
{
	id = glCreateProgram();
	for (auto shader : shaders)
	{
		glAttachShader(id, shader->id);
	}
	glLinkProgram(id);
	{
		int success; char infoLog[1024]; glad_glGetProgramiv(id, 0x8B82, &success); 
		if (!success) {
			glad_glGetShaderInfoLog(id, 1024, 0, infoLog); 
			std::cout << "ERROR:: " << "glGetProgramiv" << " " << "GL_LINK_STATUS" << infoLog << std::endl; return;
		}
	};
	for (auto shader : shaders)
	{
		glDetachShader(id, shader->id);
		//glDeleteShader(shader->id);
		
	}
}
void ShaderProgram::Active()
{
	glUseProgram(id);
}
void ShaderProgram::DisActive()
{
	glUseProgram(0);
}