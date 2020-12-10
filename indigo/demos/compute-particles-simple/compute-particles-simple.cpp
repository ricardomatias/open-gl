#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "compute-particles-simple.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GUI.h"
#include "ErrorHandler.h"

namespace fs = std::filesystem;

namespace ComputeParticlesSimple
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

	Camera cam{ glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.f), 15.f };
	GUI gui{ "#version 460" };

	
	Application::~Application()
	{
		//gui.destroy();

		LOG("[Application] destroyed");
	}

	std::string Application::getPath(const char* path) const
	{
		std::error_code ec;

		return fs::absolute(fs::path(path), ec).generic_string();
	}

	void Application::setup()
	{
		// GUI
		//gui.setup(this->getWindow());

		
		/** SHADERS **/
		std::unordered_map<ShaderTypes, std::string> shaders = {
			{ShaderTypes::VERTEX, getPath("demos/compute-particles-simple/shaders/particle.vert")},
			{ShaderTypes::FRAGMENT, getPath("demos/compute-particles-simple/shaders/particle.frag")}
		};

		m_shaderProgram->compileShaders(shaders);

		m_shaderProgram->logActiveAttributes();

		shaders = {
			{ShaderTypes::COMPUTE, getPath("demos/compute-particles-simple/shaders/particle.comp")},
		};

		m_computeProgram->compileShaders(shaders);

		m_computeProgram->logActiveAttributes();

		/** PARTICLES DATA **/
		std::vector<GLfloat> initPositions = initPosBuffer();
		std::vector<GLfloat> initVelocities(m_totalParticles * 4, 0.0f);

		const GLuint bufSize = m_totalParticles * 4 * sizeof(GLfloat);

		// The buffer for positions
		GL(glGenBuffers(1, &m_posBuf));
		GL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf));
		GL(glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initPositions[0], GL_DYNAMIC_DRAW));

		// The buffer for velocities
		GL(glGenBuffers(1, &m_velBuf));
		GL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velBuf));
		GL(glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initVelocities[0], GL_DYNAMIC_COPY));

		// Particles VAO
		GL(glGenVertexArrays(1, &m_particlesVAO));
		GL(glBindVertexArray(m_particlesVAO));

		GL(glBindBuffer(GL_ARRAY_BUFFER, m_posBuf));

		GL(glEnableVertexAttribArray(0));
		GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr));

		GL(glBindVertexArray(0));

		// Set up a buffer and a VAO for drawing the attractors (the "black holes"
		glGenBuffers(1, &m_blackHoleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_blackHoleVBO);

		GLfloat data[] = {
			m_blackHole1.x, m_blackHole1.y, m_blackHole1.z, m_blackHole1.w,
			m_blackHole2.x, m_blackHole2.y, m_blackHole2.z, m_blackHole2.w
		};

		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &m_blackHoleVAO);
		glBindVertexArray(m_blackHoleVAO);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void Application::update(double t)
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		m_angle += m_speed * deltaTime;

		if (m_angle > 360.0f) m_angle -= 360.0f;
	}

	void Application::draw()
	{
		GLFWwindow* window = getWindow();

		cam.keyboardListen(window, deltaTime);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetCursorPosCallback(window, mouseCallback);

		/** COMPUTE SHADER **/
		// Rotate the attractors ("black holes")
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(m_angle), glm::vec3(0, 0, 1));
		glm::vec3 att1 = glm::vec3(rot * m_blackHole1);
		glm::vec3 att2 = glm::vec3(rot * m_blackHole2);

		m_computeProgram->bind();

		m_computeProgram->setUniformVec3("BlackHolePos1", att1);
		m_computeProgram->setUniformVec3("BlackHolePos2", att2);

		GL(glDispatchCompute(m_totalParticles / 1000, 1, 1));

		GL(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

		m_computeProgram->unbind();

		/** RENDER PARTICLES **/
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/** MATRICES **/
		glm::mat4 view = cam.getViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 model = glm::mat4(1.0f);

		m_shaderProgram->bind();

		glm::mat4 mv = view * model;
		glm::mat4 mvp = proj * view * model;
		glm::mat3 norm = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));

		m_shaderProgram->setUniformMat4("uModelView", mv);
		m_shaderProgram->setUniformMat4("uModelViewProj", mvp);
		m_shaderProgram->setUniformVec4("uColor", glm::vec4(1.f, 1.f, 1.f, .1f));

		/** DRAW PARTICLES **/
		GL(glPointSize(1.0f));
		GL(glBindVertexArray(m_particlesVAO));

		GL(glDrawArrays(GL_POINTS, 0, m_totalParticles));

		GL(glBindVertexArray(0));

		// Draw the attractors
		GL(glPointSize(5.0f));
		GLfloat data[] = { att1.x, att1.y, att1.z, 1.0f, att2.x, att2.y, att2.z, 1.0f };

		GL(glBindBuffer(GL_ARRAY_BUFFER, m_blackHoleVBO));
		GL(glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(GLfloat), data));
		m_shaderProgram->setUniformVec4("uColor", glm::vec4(1.f, .5f, 0.f, 1.0f));

		GL(glBindVertexArray(m_blackHoleVAO));
		GL(glDrawArrays(GL_POINTS, 0, 2));
		GL(glBindVertexArray(0));
	}

	std::vector<GLfloat> Application::initPosBuffer()
	{
		// Initial positions of the particles
		std::vector<GLfloat> initPos;

		glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

		GLfloat dx = 2.0f / (m_nParticles.x - 1),
				dy = 2.0f / (m_nParticles.y - 1),
				dz = 2.0f / (m_nParticles.z - 1);

		// We want to center the particles at (0,0,0)
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, -1));

		for (int i = 0; i < m_nParticles.x; i++)
		{
			for (int j = 0; j < m_nParticles.y; j++)
			{
				for (int k = 0; k < m_nParticles.z; k++)
				{
					p.x = i * dx;
					p.y = j * dy;
					p.z = k * dz;
					p.w = 1.0f;

					p = translation * p;

					initPos.push_back(p.x);
					initPos.push_back(p.y);
					initPos.push_back(p.z);
					initPos.push_back(p.w);
				}
			}
		}
		return initPos;
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
