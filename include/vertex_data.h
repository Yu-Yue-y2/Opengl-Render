#ifndef VERTEX_H
#define VERTEX_H
#include <glad/glad.h>
#include <vector>
#include <iostream>
#include "macro.h"
class VerData
{
public:
	unsigned int vbo = 0;
	unsigned int tri_n = 0;
	unsigned int ver_size = 0;
	std::vector<float> vertices;
	int ver_n = 0;

	~VerData();
	void Bind() const;
	void DisBind() const;
	void LoadDate(const std::vector<float>& _vertices, int _size, int ver_size);
	void Analyse(unsigned int layout, int size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const;
	VerData();
};
class UniformBlock {
public:
	static unsigned int bindpoint;
	unsigned int ubo = 0;
	unsigned int private_bindpoint = 0;
	const char* name = nullptr;
	
	void Active() const;
	void DisActive() const;
	void SetSize(int size) const;
	void Bind() const;
	UniformBlock();
	~UniformBlock();
	void SetName(const char* _name);
};

class ElementData {
public:
	unsigned int ebo = 0;
	std::vector<unsigned int> indices;
	int ind_n = 0;
	ElementData();
	~ElementData();

	void Active() const;
	void DisActive() const;

	void LoadDate(const std::vector<unsigned int>& _ind, int _size);
	void Bind();
};

class VertexArray
{
public:
	unsigned int vao = 0;
	int tri_n = 0;
	std::vector<uPtr<VerData>> vbos;
	uPtr<ElementData> ebo = nullptr;
	VertexArray();
	~VertexArray();
	void Active() const;
	void Bind332();
	void Bind3();
	void Bind22();
	void DisActive()const;
};
extern std::vector<float>  test_vertices;
extern std::vector<float> quadVertices;
extern std::vector<float> skyboxVertices;
#endif