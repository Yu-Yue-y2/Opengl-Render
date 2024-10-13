#include <glad/glad.h>
#include <iostream>
#include "macro.h"
#include "vertex_data.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
unsigned int UniformBlock::bindpoint = 0;
std::vector<float> test_vertices = {
	// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};
std::vector<float> skyboxVertices = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

std::vector<float> quadVertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions         // texCoords
	-1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	 1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 1.0f,  1.0f,  0.0f, 1.0f, 1.0f
};
//VerData
VerData::~VerData()
{
	glDeleteBuffers(1, &vbo);
}
void VerData::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, ver_n * ver_size * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}
void VerData::DisBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
VerData::VerData()
{
	glGenBuffers(1, &vbo);
}
//vertices:顶点数据
//ver_n 顶点数量
//ver_size 每个顶点占多少float
void VerData::LoadDate(const std::vector<float> &_vertices, int _ver_n, int _ver_size)
{
	if (ver_size != 0 && ver_size != _ver_size)
	{
		std::cout << "not compare data vbo" << std::endl;
		return;
	}
	ver_n += _ver_n;
	ver_size = _ver_size;
	//默认三角形
	tri_n += _ver_n / 3;
	if (!vertices.empty())
	{
		//两个size相加会使得insert后capcity==size
		vertices.reserve(vertices.capacity() + _vertices.size());
		vertices.insert(vertices.end(), _vertices.begin(), _vertices.end());
	}
	else 
	{
		vertices = _vertices;
	}
}
void VerData::Analyse(unsigned int layout, int size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const
{
	glVertexAttribPointer(LAYOUT_(layout), size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(LAYOUT_(layout));
}

//UniformBlock
UniformBlock::UniformBlock()
{
	glGenBuffers(1, &ubo);
	private_bindpoint = bindpoint;
	bindpoint++;
}
void UniformBlock::Active() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
}
void UniformBlock::DisActive() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void UniformBlock::SetSize(int size) const
{
	Active();
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	DisActive();
}
void UniformBlock::SetName(const char* _name)
{
	name = _name;
}
void UniformBlock::Bind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, private_bindpoint, ubo);
}
UniformBlock::~UniformBlock() {}

//element
ElementData::ElementData() 
{
	glGenBuffers(1, &ebo);
}
ElementData::~ElementData()
{
	glDeleteBuffers(1, &ebo);
}
void ElementData::Active() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}
void ElementData::Bind()
{
	Active();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_n * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}
void ElementData::DisActive() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void ElementData::LoadDate(const std::vector<unsigned int>& _ind, int _size)
{
	indices = _ind;
	ind_n = _size;
}

//VertexArray
VertexArray::VertexArray() 
{
	glGenVertexArrays(1, &vao);
}
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vao);
}


void VertexArray::Active() const
{
	glBindVertexArray(vao);
}
void VertexArray::Bind22()
{
	Active();
	if (ebo)
	{
		ebo->Bind();
		ebo->DisActive();
	}
	int id = 0;
	for (const uPtr<VerData> &vbo : vbos)
	{
		tri_n += vbo->tri_n;
		vbo->Bind();
		vbo->Analyse(LAYOUT_(id++), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		vbo->Analyse(LAYOUT_(id++), 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		vbo->DisBind();
	}
}
void VertexArray::Bind3()
{
	Active();
	if (ebo)
	{
		ebo->Bind();
		ebo->DisActive();
	}
	int id = 0;
	for (const uPtr<VerData>& vbo : vbos)
	{
		tri_n += vbo->tri_n;
		vbo->Bind();
		vbo->Analyse(LAYOUT_(id++), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		vbo->DisBind();
	}
}
void VertexArray::Bind332()
{
	Active();
	if (ebo)
	{
		ebo->Bind();
		ebo->DisActive();
	}
	int id = 0;
	for (const uPtr<VerData>& vbo : vbos)
	{
		tri_n += vbo->tri_n;
		vbo->Bind();
		vbo->Analyse(LAYOUT_(id++), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		vbo->Analyse(LAYOUT_(id++), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		vbo->Analyse(LAYOUT_(id++), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		vbo->DisBind();
	}
	DisActive();
}
void VertexArray::DisActive() const
{
	glBindVertexArray(0);
}
