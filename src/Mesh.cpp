#include "Mesh.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	Setup();
}

void Mesh::Setup()
{
	GL(glGenVertexArrays(1, &VAO));
	GL(glGenBuffers(1, &VBO));
	GL(glGenBuffers(1, &EBO));

	GL(glBindVertexArray(VAO));

	GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));

	GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW));

	// vertex positions
	glEnableVertexAttribArray(0);
	GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr));

	// vertex normals
	glEnableVertexAttribArray(1);
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal))));
	
	// vertex tecCoords
	glEnableVertexAttribArray(2);
	GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords))));

	GL(glBindVertexArray(0)); // zero to break the existing vertex array object binding
}

void Mesh::Draw(std::shared_ptr<ShaderProgram> &shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		GL(glActiveTexture(textures[i].getTexSlot())); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		
		std::string number;
		std::string type = textures[i].getType();

		if (type == Texture::DIFFUSE)
		{
			number = std::to_string(diffuseNr++);
		}
		else if (type == Texture::SPECULAR)
		{
			number = std::to_string(specularNr++);
		}

		shader->setUniformi(("material." + type + number).c_str(), i);

		GL(glBindTexture(GL_TEXTURE_2D, textures[i].ID()));
	}

	GL(glActiveTexture(GL_TEXTURE0));

	// DRAW MESH
	GL(glBindVertexArray(VAO));

	GL(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr));

	GL(glBindVertexArray(0)); // zero to break the existing vertex array object binding
}
