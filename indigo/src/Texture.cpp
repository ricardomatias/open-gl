#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "ErrorHandler.h"
#include "Texture.h"


const std::string Texture::DEFAULT = "texture";
const std::string Texture::DIFFUSE = "diffuseTex";
const std::string Texture::SPECULAR = "specularTex";
const std::string Texture::REFLECTION = "reflectionTex";

void Texture::load(bool gamma)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(m_path.c_str(), &width, &height, &nrChannels, 0);

	GLuint texture;
	GL(glGenTextures(1, &texture));

	m_id = texture;

	if (data)
	{
		GLenum format = 0;

		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		if (m_texSlot != 0)
		{
			GL(glActiveTexture(m_texSlot));
		}
		GL(glBindTexture(GL_TEXTURE_2D, texture));

		GL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
		GL(glGenerateMipmap(GL_TEXTURE_2D));

		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

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

void Texture::unbind()
{
	GLenum textures[] = { m_texSlot };

	std::cout << "[Texture] Unbinding [" << std::hex << m_texSlot << "]" << std::endl;

	GL(glDeleteTextures(1, textures));
}
