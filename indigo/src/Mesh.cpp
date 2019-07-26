#include "Mesh.h"
#include "ErrorHandler.h"


Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<TexturePointer> &textures)
	: m_vertices(vertices), m_indices(indices), m_textures(textures)
{
	GL(glGenVertexArrays(1, &VAO));
	GL(glGenBuffers(1, &VBO));
	GL(glGenBuffers(1, &EBO));

	GL(glBindVertexArray(VAO));

	GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL(glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW));

	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW));

	// vertex positions
	glEnableVertexAttribArray(0);
	GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr));

	// vertex normals
	glEnableVertexAttribArray(1);
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal))));

	// vertex tecCoords
	glEnableVertexAttribArray(2);
	GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords))));

	// vertex tangent
	glEnableVertexAttribArray(3);
	GL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent))));
	
	// vertex bitangent
	glEnableVertexAttribArray(4);
	GL(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bitangent))));

	GL(glBindVertexArray(0)); // zero to break the existing vertex array object binding
}

void Mesh::Draw(std::shared_ptr<ShaderProgram> &shader)
{
	shader->bind();

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int reflectionNr = 1;
	unsigned int normalNr = 1;

	for (int i = 0; i < m_textures.size(); i++)
	{
		const auto texture = m_textures[i];
		
		std::string number;
		std::string type = texture->getType();

		if (type == Texture::DIFFUSE)
		{
			number = std::to_string(diffuseNr++);
		}
		else if (type == Texture::SPECULAR)
		{
			number = std::to_string(specularNr++);
		}
		else if (type == Texture::REFLECTION)
		{
			number = std::to_string(reflectionNr++);
		}
		else if (type == Texture::NORMAL)
		{
			number = std::to_string(normalNr++);
		}

		std::string uniform = "material." + type + number;

		shader->setUniformi(uniform, i);

		texture->bind();
	}

	// DRAW MESH
	GL(glBindVertexArray(VAO));

	GL(glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr));

	GL(glBindVertexArray(0)); // zero to break the existing vertex array object binding
}
