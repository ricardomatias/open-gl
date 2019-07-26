#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "demo.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GUI.h"
#include "ErrorHandler.h"

namespace DemoTemplate
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

	Camera cam{ glm::vec3(-5.f, 1.f, 40.f), glm::vec3(0.f), 15.f };
	GUI gui{ "#version 450" };

	glm::vec3 translation = glm::vec3(0.1f);

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
			{ShaderTypes::VERTEX, "res/shaders/model/basic.vert"},
			{ShaderTypes::FRAGMENT, "res/shaders/lighting/directional.frag"}
		};

		m_shaderProgram->compileShaders(shaders);

		/** MODELS **/
		//std::error_code ec;

		/** TEXTURES **/

		/** UNIFORMS **/

		// draw as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void Application::draw()
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		GLFWwindow* window = getWindow();

		cam.keyboardListen(window, deltaTime);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetCursorPosCallback(window, mouseCallback);

		/** FIRST PASS **/
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/** MATRICES **/
		glm::mat4 view{ cam.getViewMatrix() };
		glm::mat4 proj = glm::perspective(glm::radians(fov), SCR_WIDTH / static_cast<float>(SCR_HEIGHT), .1f, 300.f);

		/** PLANET DRAW **/
		m_shaderProgram->bind();

		m_shaderProgram->setUniformMat4("uProj", proj);
		m_shaderProgram->setUniformMat4("uView", view);


		glm::mat4 model(glm::mat4(1.f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.0f));

		glm::mat3 normalMatrix{ glm::transpose(glm::inverse(model)) };

		m_shaderProgram->setUniformMat4("uModel", model);
		m_shaderProgram->setUniformMat3("uNormalMatrix", normalMatrix);

		m_shaderProgram->setUniformVec3("light.direction", glm::vec3(-.2f, -10.f, -2.5f));
		m_shaderProgram->setUniformVec3("light.ambient", glm::vec3(.2f));
		m_shaderProgram->setUniformVec3("light.diffuse", glm::vec3(.9f));
		m_shaderProgram->setUniformVec3("light.specular", glm::vec3(.7f));

		m_shaderProgram->setUniformVec3("uViewPos", cam.getPosition());

		m_shaderProgram->setUniformf("material.shininess", 32.f);

		/** DRAW **/
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

		cam.lookAround(xOffset, yOffset);
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
