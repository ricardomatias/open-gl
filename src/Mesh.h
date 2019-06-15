#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "ErrorHandler.h"
#include "ShaderProgram.h"
#include "Texture.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/*  Functions  */
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	~Mesh() { std::cout << "[Mesh] destroyed" << std::endl; };

	void Draw(std::shared_ptr<ShaderProgram> &shader);

private:
	GLint m_id;
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	/*  Functions    */
	void Setup();
};

