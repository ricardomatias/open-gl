#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "ErrorHandler.h"
#include "Texture.h"


const std::string Texture::DEFAULT = "tex0";
const std::string Texture::DIFFUSE = "diffuseMap";
const std::string Texture::SPECULAR = "specularMap";
const std::string Texture::REFLECTION = "reflectionMap";
const std::string Texture::NORMAL = "normalMap";


void Texture::create(int width, int height, GLenum format, GLenum internalFormat)
{
	m_width = width;
	m_height = height;
	m_format = format;
	m_internalFormat = internalFormat;

	GL(glGenTextures(1, &m_id));

	GL(glBindTexture(GL_TEXTURE_2D, m_id));
	GL(glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height));

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	unbind();
}

void Texture::load(bool gamma)
{
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(m_path.c_str(), &m_width, &m_height, &m_nrChannels, 0);

	GLuint texture;
	GL(glGenTextures(1, &texture));

	m_id = texture;

	if (data)
	{
		m_format = 0;

		if (m_nrChannels == 1)
		{
			m_format = GL_RED;
			m_internalFormat = GL_R8;
		}
		else if (m_nrChannels == 3)
		{
			m_format = GL_RGB;
			m_internalFormat = GL_RGB8;
		}
		else if (m_nrChannels == 4)
		{
			m_format = GL_RGBA;
			m_internalFormat = GL_RGBA8;
		}

		if (m_texSlot != 0)
		{
			GL(glActiveTexture(m_texSlot));
		}

		GL(glBindTexture(GL_TEXTURE_2D, m_id));
		GL(glTexStorage2D(GL_TEXTURE_2D, 1, m_internalFormat, m_width, m_height));
		GL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data));

		//GL(glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data));
		GL(glGenerateMipmap(GL_TEXTURE_2D));

		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		unbind();

		stbi_image_free(data); // free memory
	}
	else
	{
		LOG("Failed to load texture");
		stbi_image_free(data); // free memory

	}
}

void Texture::bind()
{
	GL(glActiveTexture(m_texSlot));
	GL(glBindTexture(GL_TEXTURE_2D, m_id));
}

void Texture::bindImageTexture(GLuint uniformLocation, GLenum operationType)
{
	GL(glBindImageTexture(uniformLocation, m_id, 0, GL_FALSE, 0, operationType, m_internalFormat));
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::destroy()
{
	GLenum textures[] = { m_texSlot };

	std::cout << "[Texture] Destroying [" << std::hex << m_texSlot << "]" << std::endl;

	GL(glDeleteTextures(1, textures));
}
