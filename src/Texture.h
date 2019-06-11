#pragma once

#include <string>
#include <GL/glew.h>

class Texture
{
private:
	GLuint m_id;
	std::string m_path;
	GLint m_texSlot;
public:
	Texture(const std::string& imgPath, GLint textureSlot, GLint pixelFormat);
};
