#pragma once

#include "OpenGL.h"

class DepthMap
{
	GLuint m_id{};

	int m_width{};
	int m_height{};
	GLuint m_vao{};
	GLuint m_vbo{};
	GLuint m_texture{};

	void createFrame();
	void createTexture();
public:
	DepthMap(int width, int height);
	~DepthMap();

	void prepCapture();
	void drawFrame();

	static bool isComplete();

	void bind() const;
	void bindDepthMap(GLenum texSlot) const;

	void unbind() const;
	void destroy() const;
};
