#pragma once

#include "OpenGL.h"

class Renderer
{
	GLFWwindow* m_window;

	const char* m_title;
public:
	Renderer(int winWidth, int winHeight, const char* title);
	virtual ~Renderer();

	GLFWwindow* getWindow() const { return m_window; };

	void run();

	virtual void setup() = 0;
	virtual void draw() = 0;

	int m_windowWidth;
	int m_windowHeight;
	const char* glslVersion;
};