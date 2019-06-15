#pragma once
#define DEBUG

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) \
    if (!(x))     \
    __debugbreak();

#ifdef DEBUG
#define GL(x)   \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GL(x) x;
#endif

#define LOG(x) std::cout << (x) << std::endl

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

const char* GetGLErrorStr(GLenum err);

static void GLAPIENTRY glDebugCallback(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam
)
{
	std::string msgSource;

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:
			msgSource = "WINDOW_SYSTEM";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			msgSource = "SHADER_COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			msgSource = "THIRD_PARTY";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			msgSource = "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			msgSource = "OTHER";
			break;
	}

	std::string msgType;

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			msgType = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			msgType = "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			msgType = "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			msgType = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			msgType = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			msgType = "OTHER";
			break;
	}

	std::string msgSeverity;
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_LOW:
			msgSeverity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			msgSeverity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			msgSeverity = "HIGH";
			break;
	}

	printf("glDebugMessage:\n%s \n type = %s source = %s severity = %s\n", message, msgType.c_str(), msgSource.c_str(), msgSeverity.c_str());
}

static void glfwErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}