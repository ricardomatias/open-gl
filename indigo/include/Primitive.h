#pragma once

#include <glm/vec3.hpp>

#include "OpenGL.h"
#include <vector>

enum class PrimitiveType
{
	QUAD,
	PLANE,
	CUBE
};

class Primitive
{
	GLuint m_vao, m_vbo;
	unsigned int m_vertices;
	PrimitiveType m_type;

	const std::vector<float> getPrimitiveData();
public:
	Primitive(PrimitiveType type, const glm::vec3 pos, const glm::vec3 rvec, float rangle, const glm::vec3 scl)
		: m_vao(0), m_vbo(0), m_vertices(0), m_type(type), position(pos), rotVec(rvec), rotAngle(rangle), scale(scl)
	{
		create();
	}

	Primitive(PrimitiveType type)
		: m_vao(0), m_vbo(0), m_vertices(0), m_type(type), position(glm::vec3(0.f)), rotVec(glm::vec3(0.f)), rotAngle(0.f), scale(glm::vec3(1.f))
	{
		create();
	};

	~Primitive();

	void create();

	void bind();
	void unbind();
	
	void draw();

	glm::vec3 position;
	glm::vec3 rotVec;
	float rotAngle;
	glm::vec3 scale;

	static const std::vector<float> QUAD;
	static const std::vector<float> PLANE;
	static const std::vector<float> CUBE;
};

typedef std::shared_ptr<Primitive> PrimitivePointer;