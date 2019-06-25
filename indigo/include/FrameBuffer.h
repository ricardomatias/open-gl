#pragma once

#include "OpenGL.h"

class FrameBuffer
{
	GLuint m_id{};
	GLuint m_texture;
	GLuint m_renderBuffer;

	int m_width;
	int m_height;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_depthMap;
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();
	void createFrame();
	void drawFrame();

	static bool isComplete();

	void bind() const;
	
	void unbind() const;
	void destroy() const;
	
	void createTextureAttach();
	void bindTexture();
	void unbindTexture();

	void createRenderBufferAttach();
	void bindRenderBuffer();
	void unbindRenderBuffer();
};
