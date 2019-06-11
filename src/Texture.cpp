#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "ErrorHandler.h"
#include "Texture.h"

Texture::Texture(const std::string &imgPath, GLint textureSlot, GLint pixelFormat)
	: m_path(imgPath), m_texSlot(textureSlot)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(imgPath.c_str(), &width, &height, &nrChannels, 0);

	GLuint texture;
	GL(glGenTextures(1, &texture));

	m_id = texture;

	GL(glActiveTexture(textureSlot));
	GL(glBindTexture(GL_TEXTURE_2D, texture));

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	if (data)
	{

		GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data));
		GL(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		LOG("Failed to load texture");
	}

	stbi_image_free(data); // free memory
}