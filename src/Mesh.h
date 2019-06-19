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
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<TexturePointer> m_textures;

	/*  Functions  */
	Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<TexturePointer> &textures);
	~Mesh() {
		m_vertices.clear();
		m_indices.clear();
		m_textures.clear();

		std::cout << "[Mesh] destroyed" << std::endl;
	};

	void Draw(std::shared_ptr<ShaderProgram> &shader);

private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
};

