#include "DepthMap.h"
#include "ErrorHandler.h"

static const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

DepthMap::DepthMap(int width, int height)
	: m_width(width), m_height(height), m_vao(0), m_vbo(0), m_texture(0)
{
	GL(glGenFramebuffers(1, &m_id));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

	createFrame();

	createTexture();

	if (!isComplete())
	{
		std::cerr << "[DepthMap] could not create complete" << std::endl;
	}
}

DepthMap::~DepthMap()
{
	destroy();

	std::cout << "[DepthMap] destroyed" << std::endl;
}

bool DepthMap::isComplete()
{
	return GL(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void DepthMap::createFrame()
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

void DepthMap::createTexture()
{
	GL(glGenTextures(1, &m_texture));
	GL(glBindTexture(GL_TEXTURE_2D, m_texture));

	GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	// with this all coordinates outside the depth map's range have a depth of 1.0 which as a result means these coordinates will never be in shadow

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	bind();

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0));

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	unbind();
}

void DepthMap::bind() const
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
}

void DepthMap::bindDepthMap(GLenum texSlot = GL_TEXTURE0) const
{
	GL(glActiveTexture(texSlot));
	GL(glBindTexture(GL_TEXTURE_2D, m_texture));
}

void DepthMap::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK); // don't forget to reset original culling face
}

void DepthMap::destroy() const
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);

	GL(glDeleteFramebuffers(1, &m_id));
}

void DepthMap::prepCapture()
{
	glViewport(0, 0, m_width, m_height);

	bind();
	
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
}

void DepthMap::drawFrame()
{
	unbind();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindVertexArray(m_vao);

	bindDepthMap();

	glDrawArrays(GL_TRIANGLES, 0, 6);
}