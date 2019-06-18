#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Cube.h"
#include "Model.h"
#include "GUI.h"
#include "Plane.h"

namespace fs = std::filesystem;

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

static double lastX = SCR_WIDTH / 2.;
static double lastY = SCR_HEIGHT / 2.;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, double deltaTime);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

static float fov = 45.f;

static bool firstMouse = true;

Camera cam{ glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f), 5.f };
GUI gui{ "#version 450" };

class Application final : public Renderer
{
	std::shared_ptr<ShaderProgram> m_meshShaders;
	std::shared_ptr<ShaderProgram> m_singleShader;

	unsigned int m_cubeVao, m_cubeVbo;
	unsigned int m_planeVao, m_planeVbo;

	std::shared_ptr<Texture> m_cubeTexture;
	std::shared_ptr<Texture> m_planeTexture;
public:
	Application(const int windowWidth, const int windowHeight)
		: Renderer{windowWidth, windowHeight},
		  m_meshShaders(std::make_shared<ShaderProgram>()),
		  m_singleShader(std::make_shared<ShaderProgram>()),
		  fps(0), avgFrame(0), frames(0), deltaTime(0), lastTime(0),
		  currentFrame(0), lastFrame(0)
	{
	};
	virtual ~Application();

	virtual void setup();
	virtual void draw();

	void drawCubes(std::shared_ptr<ShaderProgram>& shader, bool shouldScale = false);

	int fps;
	double avgFrame;
	int frames;
	double deltaTime, lastTime;
	double currentFrame, lastFrame;
};

Application::~Application()
{
	gui.destroy();

	glDeleteVertexArrays(1, &m_cubeVao);
	glDeleteVertexArrays(1, &m_planeVao);
	glDeleteBuffers(1, &m_cubeVao);
	glDeleteBuffers(1, &m_planeVbo);

	LOG("[Application] destroyed");
}

void Application::setup()
{
	// GUI
	gui.setup(this->getWindow());

	// SHADERS
	auto vert = std::make_shared<Shader>(Shader::VERTEX, std::string("res/shaders/basic/basic.vert"));
	auto frag = std::make_shared<Shader>(Shader::FRAGMENT, std::string("res/shaders/basic/stencil-test.frag"));

	std::vector<std::shared_ptr<Shader>> shaders{ std::move(vert), std::move(frag) };

	m_meshShaders->compileShaders(shaders);

	vert = std::make_shared<Shader>(Shader::VERTEX, std::string("res/shaders/basic/basic.vert"));
	frag = std::make_shared<Shader>(Shader::FRAGMENT, std::string("res/shaders/basic/border.frag"));

	shaders = std::vector<std::shared_ptr<Shader>>{ std::move(vert), std::move(frag) };

	m_singleShader->compileShaders(shaders);

	// CUBE
	glGenVertexArrays(1, &m_cubeVao);
	glBindVertexArray(m_cubeVao);

	glGenBuffers(1, &m_cubeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), &CUBE, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	
	glBindVertexArray(0);

	// PLANE
	glGenVertexArrays(1, &m_planeVao);
	glBindVertexArray(m_planeVao);

	glGenBuffers(1, &m_planeVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_planeVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE), &PLANE, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(0));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	
	glBindVertexArray(0);

	/** UNIFORMS **/
	std::error_code ec;

	const auto cubeTexPath = fs::absolute(fs::path("res/images/marble.jpg"), ec);
	const auto planeTexPath = fs::absolute(fs::path("res/images/metal.png"), ec);

	m_cubeTexture = std::make_shared<Texture>( cubeTexPath.generic_string(), GL_TEXTURE0 );
	m_planeTexture = std::make_shared<Texture>( planeTexPath.generic_string(), GL_TEXTURE0 );

	m_cubeTexture->load();
	m_planeTexture->load();

	m_meshShaders->Bind();

	m_meshShaders->setUniformi("tex0", 0);
}

void Application::draw()
{
	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	GLFWwindow* window = getWindow();

	processInput(window, deltaTime);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, mouseCallback);


	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);

	// Simply clear the window with re
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing the floor

	glm::mat4 model = glm::mat4(1.f);

	const glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
	const glm::mat4 view = lookAt(cam.getPosition(), cam.getLookAt(), glm::vec3(0, 1, 0));

	glm::vec3 lightPosition(0.f, 2.f, -2.f);

	m_meshShaders->Bind();

	m_meshShaders->setUniformMat4("uProj", projection);
	m_meshShaders->setUniformMat4("uView", view);

	/** FLOOR DRAW **/
	glBindVertexArray(m_planeVao);
	glBindTexture(GL_TEXTURE_2D, m_planeTexture->ID());

	m_meshShaders->setUniformMat4("uModel", glm::mat4(1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	/** CUBE DRAW **/
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should update the stencil buffer
	glStencilMask(0xFF); // enable writing to the stencil buffer

	drawCubes(m_meshShaders);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); // disable writing to the stencil buffer
	glDisable(GL_DEPTH_TEST);

	m_singleShader->Bind();

	m_singleShader->setUniformMat4("uProj", projection);
	m_singleShader->setUniformMat4("uView", view);

	drawCubes(m_singleShader, true);

	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);


	gui.createFrame();

	{
		// Measure speed
		frames++;

		if (currentFrame - lastTime >= 1.0)
		{ // If last prinf() was more than 1 sec ago
		  // printf and reset timer
			avgFrame = 1000.0 / double(frames);
			fps = frames;
			frames = 0;
			lastTime += 1.0;
		}

		ImGui::Begin("Hello, world!");
		ImGui::Text("FPS: %i", fps);
		ImGui::Text("Avg Frame: %f", avgFrame);
		ImGui::End();
	}

	gui.draw();
}

void Application::drawCubes(std::shared_ptr<ShaderProgram> &shader, bool shouldScale)
{
	glm::mat4 model{ 1.f };

	shader->Bind();

	glBindVertexArray(m_cubeVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_cubeTexture->ID());

	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));

	if (shouldScale)
	{
		model = glm::scale(model, glm::vec3(1.05f));
	}

	shader->setUniformMat4("uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));

	if (shouldScale)
	{
		model = glm::scale(model, glm::vec3(1.05f));
	}

	shader->setUniformMat4("uModel", model);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main()
{
	Application app{ SCR_WIDTH, SCR_HEIGHT };

	app.run();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, const double deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.move(CameraDirection::FORWARD, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.move(CameraDirection::BACKWARD, static_cast<float>(deltaTime));

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cam.move(CameraDirection::UP, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cam.move(CameraDirection::DOWN, static_cast<float>(deltaTime));

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.strafe(CameraDirection::LEFT, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.strafe(CameraDirection::RIGHT, static_cast<float>(deltaTime));
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