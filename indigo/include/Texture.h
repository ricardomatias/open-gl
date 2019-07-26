#pragma once

#include <string>
#include <iostream>
#include <utility>

#include "OpenGL.h"


enum class TextureType
{
	DIFFUSE,
	SPECULAR,
	REFLECTION,
	NORMAL,
	DEFAULT
};

class Texture
{
	GLuint m_id;
	std::string m_path;
	std::string m_type;
	GLenum m_texSlot;

	GLenum m_format, m_internalFormat;
	int m_width, m_height, m_nrChannels;
public:
	Texture()
		: m_id(0), m_path(std::string("")), m_type(DEFAULT), m_texSlot(GL_TEXTURE0), m_format(GL_RGB), m_internalFormat(GL_RGB8), m_width(0), m_height(0), m_nrChannels(0) {};

	Texture(std::string imgPath, std::string type)
		: m_id(0), m_path(std::move(imgPath)), m_type(std::move(type)), m_texSlot(GL_TEXTURE0), m_format(GL_RGB), m_internalFormat(GL_RGB8), m_width(0), m_height(0), m_nrChannels(0) {};
	
	Texture(std::string imgPath, const GLenum texSlot)
		: m_id(0), m_path(std::move(imgPath)), m_type(DEFAULT), m_texSlot(texSlot), m_format(GL_RGB), m_internalFormat(GL_RGB8), m_width(0), m_height(0), m_nrChannels(0) {};

	~Texture() {
		destroy();
	};

	GLuint ID() const { return m_id; };
	std::string getType() const { return m_type; };

	std::string getPath() const { return m_path; };
	void setPath(const std::string& imgPath) { m_path = imgPath; };

	GLuint getTexSlot() const { return m_texSlot; };
	void setTexSlot(GLenum texSlot) { m_texSlot = texSlot; };

	int getWidth() const { return m_width; };
	int getHeight() const { return m_height; };
	GLenum getFormat() const { return m_format; };
	GLenum getInternalFormat() const { return m_internalFormat; };


	void load(bool gamma = false);
	void create(int width, int height, GLenum format, GLenum internalFormat);

	void bind();
	void bindImageTexture(GLuint uniformLocation, GLenum operationType);
	void unbind();
	void destroy();

	static const std::string DEFAULT;
	static const std::string DIFFUSE;
	static const std::string SPECULAR;
	static const std::string REFLECTION;
	static const std::string NORMAL;
};

typedef std::shared_ptr<Texture> TexturePointer;
