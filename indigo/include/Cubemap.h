#pragma once

#include <vector>

#include "OpenGL.h"
#include "ShaderProgram.h"
#include "Camera.h"

class Cubemap
{
	GLuint m_id;
	GLuint m_vao;
	GLuint m_vbo;
	std::shared_ptr<ShaderProgram> m_shaderProgram;
public:
	Cubemap();
	~Cubemap();

	void load(const std::string& dir);

	void bind();
	void unbind();

	void createCube();
	void draw(glm::mat4 &proj, glm::mat4 camViewMatrix);
};
