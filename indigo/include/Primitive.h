#pragma once

#include <glm/vec3.hpp>

#include "OpenGL.h"
#include <vector>


class Primitive
{
	GLuint m_vao, m_vbo;
	unsigned int m_vertices;

public:
	Primitive(const std::vector<float> &primitive, const glm::vec3 pos, const glm::vec3 rvec, float rangle, const glm::vec3 scl)
		: m_vertices(primitive.size() / 8), position(pos), rotVec(rvec), rotAngle(rangle), scale(scl)
	{
		create(primitive);
	}

	Primitive(const std::vector<float> &primitive)
		: m_vertices(primitive.size() / 8), position(glm::vec3(0.f)), rotVec(glm::vec3(0.f)), rotAngle(0.f), scale(glm::vec3(1.f))
	{
		create(primitive);
	};

	~Primitive();

	void create(const std::vector<float>& primitive);

	void bind();
	void unbind();
	
	void draw();

	glm::vec3 position;
	glm::vec3 rotVec;
	float rotAngle;
	glm::vec3 scale;

	static const std::vector<float> PLANE;
	static const std::vector<float> CUBE;
};

typedef std::shared_ptr<Primitive> PrimitivePointer;