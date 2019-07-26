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
	DEFAULT
};

class Texture
{
	GLuint m_id;
	std::string m_path;
	std::string m_type;
	GLenum m_texSlot;
public:
	Texture(std::string imgPath, std::string type)
		: m_id(0), m_path(std::move(imgPath)), m_type(std::move(type)), m_texSlot(GL_TEXTURE0) {};
	
	Texture(std::string imgPath, const GLenum texSlot)
		: m_id(0), m_path(std::move(imgPath)), m_type(DEFAULT), m_texSlot(texSlot) {};

	~Texture() {
		unbind();
	};

	GLuint ID() const { return m_id; };
	std::string getType() const { return m_type; };
	std::string getPath() const { return m_path; };

	GLuint getTexSlot() const { return m_texSlot; };
	void setTexSlot(GLenum texSlot) { m_texSlot = texSlot; };

	void load(bool gamma = false);

	void bind();
	void unbind();

	static const std::string DEFAULT;
	static const std::string DIFFUSE;
	static const std::string SPECULAR;
	static const std::string REFLECTION;
};

typedef std::shared_ptr<Texture> TexturePointer;
