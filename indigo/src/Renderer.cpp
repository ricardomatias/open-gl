#include <iostream>

#include "Renderer.h"
#include "ErrorHandler.h"


Renderer::Renderer(int winWidth, int winHeight, const char* title)
	: m_windowWidth(winWidth), m_windowHeight(winHeight), m_window(nullptr), m_title(title)
{
	glslVersion = "#version 450";

	glfwSetErrorCallback(glfwErrorCallback);

	/* Initialize the library */
	if (!glfwInit())
		return; // -1 | throw exception

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4); // ANTI-ALIASING

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_title, NULL, NULL);

	if (!m_window)
	{
		glfwTerminate();
		return; // -1 | throw exception
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(m_window);

	//glfwSwapInterval(1); // V-SYNC

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	std::cout << glGetString(GL_VERSION) << std::endl;

	/* OPEN-GL DEFAULTS */

	// Set debug callback
	glEnable(GL_DEBUG_OUTPUT);

	if (glDebugMessageCallback != NULL)
	{
		glDebugMessageCallback(glDebugCallback, NULL);
	}

	glEnable(GL_MULTISAMPLE);
}

Renderer::~Renderer()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();

	LOG("[Renderer] destroyed");
}

void Renderer::setup() {};
void Renderer::draw() {};

void Renderer::run()
{
	setup();

	int frames = 0;
	int fps = 0;
	double lastTime = 0.0;
	double frameTime = 0.0;


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(m_window))
	{
		draw();

		const double currentFrame = glfwGetTime();

		// Measure speed
		frames++;

		if (currentFrame - lastTime >= 1.0)
		{ // If last prinf() was more than 1 sec ago
		  // printf and reset timer
			frameTime = 1000.0 / double(frames);
			fps = frames;
			frames = 0;
			lastTime += 1.0;

			char buffer[256];

			sprintf_s(buffer, "%s - FPS: %i - Frame Time: %.3f ms", m_title, fps, frameTime);

			glfwSetWindowTitle(m_window, buffer);
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(m_window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}