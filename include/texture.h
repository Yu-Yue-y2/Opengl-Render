#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include<iostream>
#include <vector>
#include "macro.h"
class Texture {
public:
	unsigned int id = 0;
	int width, height;
	GLenum color_format1, color_format2;
	const char* name;
	Texture();
	//depth: GL_DEPTH_COMPONENT GL_DEPTH_COMPONENT
	Texture(int _width, int _height, GLenum _color_format1 = GL_RGB, GLenum _color_format2 = GL_RGB);
	Texture(const char* path, const char* _name, bool srgb = true, bool reverse_y = true);
	virtual void Active();
	virtual void DisActive();
	virtual void bind();
	virtual void Load(const char* path,bool srgb = true, bool reverse_y = true);
	void Resize(int _width, int _height);
	void AttachDepth();
	void SetName(const char* _name);
};

class CubeTexture :public Texture {
public:
	CubeTexture(std::vector<std::string>& paths) {
		Load(paths);
	}
	void Load(std::vector<std::string>&);
	void Active()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}
	void DisActive()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	void bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}

};
class FrameBuffer {
public:
	unsigned int fbo;
	int width, height;
	uPtr<Texture> depthTexture;
	std::vector<uPtr<Texture>> colors;
	FrameBuffer(int _width, int _height, bool nocolor = false, GLuint color_format = GL_RGB)
		: width(_width), height(_height)
	{
		glGenFramebuffers(1, &fbo);
		depthTexture = mkU<Texture>(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
		depthTexture->SetName("shadowmap");
		if (!nocolor) 
		{
			uPtr<Texture> color = mkU<Texture>(width, height, color_format);
			color->SetName("fbo_color");
			colors.push_back(std::move(color));
		}
		Bind();
	}
	void Resize(int _width, int _height)
	{
		width = _width;
		height = _height;
		for (uPtr<Texture>& color : colors) 
		{
			color->Resize(width, height);
		}
		if (depthTexture) 
		{
			depthTexture->Resize(width, height);
		}
	}
	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &fbo);
	}
	inline void Active()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	inline void DisActive()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Bind()
	{
		Active();
		int i = 0;
		for (uPtr<Texture>& color : colors)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color->id, 0);
			i++;
		}
		if (colors.size() == 0)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		if (depthTexture) 
		{
			depthTexture->AttachDepth();
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "fbo create fail" << std::endl;
		}
		DisActive();

	}
};
#endif