#include <stb/stb_image.h>

#include "Cubemap.h"
#include "Cube.h"
#include "Shader.h"
#include "Camera.h"

Cubemap::Cubemap()
	: m_id(0), m_vbo(0), m_shaderProgram(std::make_shared<ShaderProgram>())
{
	auto vert = std::make_shared<Shader>(Shader::VERTEX, std::string("res/shaders/cubemap.vert"));
	auto frag = std::make_shared<Shader>(Shader::FRAGMENT, std::string("res/shaders/cubemap.frag"));

	auto shaders = std::vector<std::shared_ptr<Shader>>{std::move(vert), std::move(frag)};

	m_shaderProgram->compileShaders(shaders);

	m_shaderProgram->bind();

	m_shaderProgram->setUniformi("texCube", 0);
}

Cubemap::~Cubemap()
{
	unbind();

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	std::cout << "[Cubemap] destroyed" << std::endl;
}

void Cubemap::load(const std::string &dir, std::vector<std::string> &faces)
{
	GL(glGenTextures(1, &m_id));
	GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_id));

	int width, height, nrChannels;

	for (unsigned int i = 0; i < faces.size(); ++i)
	{
		std::string filepath = dir + faces[i];

		try
		{
			unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

			if (data == nullptr) throw std::runtime_error("[Cubemap] Could not load " + filepath);

			GL(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			));
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

void Cubemap::bind()
{
	GL(glActiveTexture(GL_TEXTURE_CUBE_MAP));
	GL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_id));
}

void Cubemap::unbind()
{
	GLenum textures[] = { m_id };

	std::cout << "[Cubemap] Unbinding [" << std::hex << m_id << "]" << std::endl;

	GL(glDeleteTextures(1, textures));
}

void Cubemap::createCube()
{
	GL(glGenVertexArrays(1, &m_vao));
	GL(glBindVertexArray(m_vao));

	GL(glGenBuffers(1, &m_vbo));
	GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
	GL(glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), &CUBE, GL_STATIC_DRAW));

	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr));

	GL(glEnableVertexAttribArray(0));
	GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))));

	GL(glBindVertexArray(0));
}

void Cubemap::draw(const glm::mat4 &proj, glm::mat4 camViewMatrix)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	glDepthMask(GL_FALSE);

	glBindVertexArray(m_vao);

	m_shaderProgram->bind();

	glm::mat4 view = glm::mat4(glm::mat3(camViewMatrix)); // removes the translation part

	m_shaderProgram->setUniformMat4("uProj", proj);
	m_shaderProgram->setUniformMat4("uView", view);

	bind();

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);

	glDepthFunc(GL_LESS); // back to default
}
