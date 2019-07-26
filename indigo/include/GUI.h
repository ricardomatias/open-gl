#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "OpenGL.h"

class GUI
{
private:
	const char* m_glslVersion;
public:
	GUI(const char* glslVersion) : m_glslVersion(glslVersion) {};

	void setup(GLFWwindow* window);

	void destroy();

	void createFrame();
	void draw();
};


