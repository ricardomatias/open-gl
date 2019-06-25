#include <iostream>

#include "ErrorHandler.h"
#include "FrameBuffer.h"

static const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

static const GLint SAMPLES = 4;

FrameBuffer::FrameBuffer(int width, int height)
	: m_texture(0), m_renderBuffer(0), m_width(width), m_height(height), m_vao(0), m_vbo(0)
{
	GL(glGenFramebuffers(1, &m_id));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
}

FrameBuffer::~FrameBuffer()
{
	unbind();

	if (m_texture) unbindTexture();
	if (m_renderBuffer) unbindRenderBuffer();
	
	destroy();

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);

	std::cout << "[FrameBuffer] destroyed" << std::endl;
}

void FrameBuffer::createFrame()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<GLvoid*>(2 * sizeof(float)));

	glBindVertexArray(0);
}

void FrameBuffer::drawFrame()
{
	unbind();

	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindVertexArray(m_vao);

	if (m_texture) bindTexture();

	if (m_renderBuffer) bindRenderBuffer();

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool FrameBuffer::isComplete()
{
	return GL(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void FrameBuffer::bind() const
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id));
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	GL(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
}

void FrameBuffer::unbind() const
{
	//  A framebuffer object however is not complete without a color buffer
	// so we need to explicitly tell OpenGL we're not going to render any color data. 
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void FrameBuffer::destroy() const
{
	GL(glDeleteFramebuffers(1, &m_id));
}

void FrameBuffer::createTextureAttach()
{
	bind();

	GL(glGenTextures(1, &m_texture));
	GL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texture));

	GL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLES, GL_RGB, m_width, m_height, GL_TRUE));

	GL(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_texture, 0));
}

void FrameBuffer::bindTexture()
{
	GL(glBindTexture(GL_TEXTURE_2D, m_texture));
}

void FrameBuffer::unbindTexture()
{
	GLenum textures[] = { m_renderBuffer };

	GL(glDeleteTextures(1, &textures[0]));
}

void FrameBuffer::createRenderBufferAttach()
{
	bind();

	GL(glGenRenderbuffers(1, &m_renderBuffer));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer));

	GL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLES, GL_DEPTH24_STENCIL8, m_width, m_height));
	
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer));
}

void FrameBuffer::bindRenderBuffer()
{
	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer));
}


void FrameBuffer::unbindRenderBuffer() // BROKEN
{
	//GLuint renderBuffers[] = { m_renderBuffer };

	//GL(glDeleteRenderbuffers(GL_RENDERBUFFER, renderBuffers));
}