#pragma once

#include <vector>

#include "OpenGL.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Primitive.h"

class Cubemap
{
	GLuint m_id;
	std::shared_ptr<Primitive> m_cube;
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
