#ifndef SHADER_H
#define SHADER_H
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertex_data.h"
#include "texture.h"
class Shader {
public:
	unsigned int id = 0;
	Shader(unsigned int type, const char* _c);
	~Shader();
};
class ShaderProgram
{
public:
	unsigned int id = 0;
	ShaderProgram(std::vector<Shader*>& shaders);
	void Active();
	void DisActive();
	void BindUniformBlock(UniformBlock* block)
	{
		if (id == 0 || !block->name)
		{
			std::cout << "uninitial block bind" << std::endl;
			return;
		}
		unsigned int blockid = glGetUniformBlockIndex(id, block->name);
		glUniformBlockBinding(id, blockid, block->private_bindpoint);
	}
	~ShaderProgram()
	{
		glDeleteProgram(id);
	}
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}
	void setMatrix4(const std::string& name, glm::mat4 mat)
	{
		unsigned int transformLoc = glGetUniformLocation(id, name.c_str());
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}
	void setVec3(const std::string& name, glm::vec3 vec)
	{
		unsigned int transformLoc = glGetUniformLocation(id, name.c_str());
		glUniform3fv(transformLoc, 1, glm::value_ptr(vec));
		//glUniform3f(transformLoc, vec[0], vec[1], vec[2]);
	}
	void setMatrix3(const std::string& name, glm::mat3 mat)
	{
		unsigned int transformLoc = glGetUniformLocation(id, name.c_str());
		glUniformMatrix3fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}
	void setVec4(const std::string& name, glm::vec4 vec)
	{
		unsigned int transformLoc = glGetUniformLocation(id, name.c_str());
		glUniform4fv(transformLoc, 1, glm::value_ptr(vec));
	}
	void setTexture(int bindid, Texture* text)
	{
		glActiveTexture(GL_TEXTURE0 + bindid);
		text->bind();
		glUniform1i(glGetUniformLocation(id, text->name), bindid);
		/*
		1,寻找纹理在shader里的位置
		2，打算使用opengl的哪一个纹理（GL_TEXTURE0，GL_TEXTURE1。。。），并把相较于GL_TEXTURE0的索引给shader
		3,激活
		4，绑定到TEXTURE_2D,CUBE等等，意思是正在使用该确定的物理真实纹理。
		*/
		//active和bind的顺序不要搞错了
		
	}
};
#endif