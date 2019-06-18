#pragma once

#include <string>
#include <iostream>
#include <GL/glew.h>


enum class TextureType
{
	DIFFUSE,
	SPECULAR,
	DEFAULT
};

class Texture
{
private:
	GLuint m_id;
	std::string m_path;
	std::string m_type;
	GLenum m_texSlot;
public:
	Texture(const std::string& imgPath, const std::string& type)
		: m_id(0), m_path(imgPath), m_type(type), m_texSlot(GL_TEXTURE0) {};
	
	Texture(const std::string& imgPath, const GLenum texSlot)
		: m_id(0), m_path(imgPath), m_type(DEFAULT), m_texSlot(texSlot) {};

	~Texture() {
		Unbind();
	};

	GLuint ID() const { return m_id; };
	std::string getType() const { return m_type; };
	std::string getPath() const { return m_path; };

	GLuint getTexSlot() const { return m_texSlot; };
	void setTexSlot(GLenum texSlot) { m_texSlot = texSlot; };

	void load(bool gamma = false);

	void Bind();
	void Unbind();

	static const std::string DEFAULT;
	static const std::string DIFFUSE;
	static const std::string SPECULAR;
};

typedef std::shared_ptr<Texture> TexturePointer;
