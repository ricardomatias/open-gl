#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "compute.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GUI.h"
#include "ErrorHandler.h"

namespace BasicComputeDemo
{
	// settings
	static double lastX = SCR_WIDTH / 2.;
	static double lastY = SCR_HEIGHT / 2.;

	void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window, double deltaTime);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

	static float fov = 45.f;

	static bool firstMouse = true;

	//Camera cam{ glm::vec3(-5.f, 1.f, 40.f), glm::vec3(0.f), 15.f };
	GUI gui{ "#version 450" };

	glm::vec3 translation = glm::vec3(0.1f);

	#define WORKGROUP_SIZE 16

	Application::~Application()
	{
		//gui.destroy();

		LOG("[Application] destroyed");
	}

	void Application::setup()
	{
		// GUI
		//gui.setup(this->getWindow());

		/** SHADERS **/
		std::unordered_map<ShaderTypes, std::string> shaders = {
			{ShaderTypes::VERTEX, "res/shaders/primitive/quad.vert"},
			{ShaderTypes::FRAGMENT, "res/shaders/primitive/quad.frag"}
		};

		m_shaderProgram->compileShaders(shaders);

		shaders = {
			{ShaderTypes::COMPUTE, "res/shaders/basic.comp"},
		};

		m_computeProgram->compileShaders(shaders);

		/** MODELS **/
		//std::error_code ec;

		/** TEXTURES **/
		m_imageTex->setPath("res/images/flower1024.png");

		m_imageTex->load();

		m_computedTex->setTexSlot(GL_TEXTURE1);

		m_computedTex->create(m_imageTex->getWidth(), m_imageTex->getHeight(), m_imageTex->getFormat(), m_imageTex->getInternalFormat());

		/** UNIFORMS **/
		m_shaderProgram->bind();

		m_imageTex->bind();

		m_shaderProgram->setUniformi("tex0", 1);

		m_shaderProgram->unbind();

		// draw as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void Application::draw()
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		GLFWwindow* window = getWindow();

		//cam.keyboardListen(window, deltaTime);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetCursorPosCallback(window, mouseCallback);

		/** FIRST PASS **/
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/** MATRICES **/
		//glm::mat4 view{ cam.getViewMatrix() };
		glm::mat4 proj = glm::ortho(-1.0, 1.0, -1.0, 1.0);

		/** PLANET DRAW **/
		m_computeProgram->bind();

		m_imageTex->bindImageTexture(0, GL_READ_ONLY);
		m_computedTex->bindImageTexture(1, GL_WRITE_ONLY);

		GL(glDispatchCompute(m_computedTex->getWidth() / WORKGROUP_SIZE, m_computedTex->getHeight() / WORKGROUP_SIZE, 1));

		GL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		m_computeProgram->unbind();

		/** SECOND PASS **/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_shaderProgram->bind();
		m_quad->bind();

		m_computedTex->bind();

		/** DRAW **/
		m_quad->draw();
	}

	int main()
	{
		Application app{ SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial" };

		app.run();

		return 0;
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void frameBufferSizeCallback(GLFWwindow* window, const int width, const int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	void mouseCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;  // NOLINT
			lastY = ypos;
			firstMouse = false;
		}

		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		if (state != GLFW_PRESS)
		{
			return;
		}

		//cam.lookAround(xOffset, yOffset);
	}

	void scrollCallback(GLFWwindow* window, double /*offset*/, double yOffset)
	{
		if (fov >= 1.0f && fov <= 45.0f)
			fov -= static_cast<float>(yOffset);
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 45.0f)
			fov = 45.0f;
	}
}
