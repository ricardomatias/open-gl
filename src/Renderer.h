#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Renderer
{
private:
	GLFWwindow* m_window;
public:
	Renderer(int winWidth, int winHeight);
	virtual ~Renderer();

	GLFWwindow* getWindow() const { return m_window; };

	void run();

	virtual void setup() = 0;
	virtual void draw() = 0;

	int m_windowWidth;
	int m_windowHeight;
	const char* glslVersion;
};