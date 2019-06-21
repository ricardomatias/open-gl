#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Cube.h"
#include "GUI.h"
#include "Plane.h"
#include "ErrorHandler.h"
#include "Cubemap.h"
#include "Model.h"

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

Camera cam{ glm::vec3(0.f, 1.f, 3.f), glm::vec3(0.f), 5.f };
GUI gui{ "#version 450" };

class Application final : public Renderer
{
	std::shared_ptr<ShaderProgram> m_meshShaders;
	std::shared_ptr<ShaderProgram> m_lightShaders;

	unsigned int m_cubeVao, m_cubeVbo;
	unsigned int m_planeVao, m_planeVbo;

	std::shared_ptr<Model> m_nanosuit;

	std::shared_ptr<Texture> m_cubeTexture;
	std::shared_ptr<Texture> m_planeTexture;
	std::shared_ptr<Cubemap> m_cubemap;
public:
	Application(const int windowWidth, const int windowHeight, const char* title)
		: Renderer{windowWidth, windowHeight, title},
			m_meshShaders(std::make_shared<ShaderProgram>()),
			m_lightShaders(std::make_shared<ShaderProgram>()),
			m_cubeVao(0), m_cubeVbo(0), m_planeVao(0), m_planeVbo(0),
			m_nanosuit(std::make_shared<Model>()),
			m_cubemap(std::make_shared<Cubemap>()),
			deltaTime(0),
			currentFrame(0), lastFrame(0)
	{};
	virtual ~Application();

	virtual void setup();
	virtual void draw();

	double deltaTime;
	double currentFrame, lastFrame;
};

Application::~Application()
{
	//gui.destroy();

	glDeleteVertexArrays(1, &m_cubeVao);
	glDeleteBuffers(1, &m_cubeVao);

	LOG("[Application] destroyed");
}

void Application::setup()
{
	// GUI
	//gui.setup(this->getWindow());

	// SHADERS
	auto vert = std::make_shared<Shader>(Shader::VERTEX, std::string("res/shaders/model/basic.vert"));
	auto geom = std::make_shared<Shader>(Shader::GEOMETRY, std::string("res/shaders/exploding.geom"));
	auto frag = std::make_shared<Shader>(Shader::FRAGMENT, std::string("res/shaders/lighting/directional.frag"));

	std::vector<std::shared_ptr<Shader>> shaders{ std::move(vert), std::move(geom), std::move(frag) };

	m_meshShaders->compileShaders(shaders);

	/** nanosuit **/
	std::error_code ec;

	m_nanosuit->loadModel(fs::absolute(fs::path("res/models/nanosuit/nanosuit.obj"), ec).generic_string());

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

	processInput(window, deltaTime);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	/** FIRST PASS **/
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/** MATRICES **/
	glm::mat4 view{ cam.getViewMatrix() };

	glm::mat4 proj = glm::perspective(glm::radians(fov), SCR_WIDTH / static_cast<float>(SCR_HEIGHT), 0.1f, 100.f);


	/** NANOSUIT DRAW **/
	m_meshShaders->bind();

	m_meshShaders->setUniformf("uTime", static_cast<float>(currentFrame) * 2.f);

	m_meshShaders->setUniformMat4("uProj", proj);
	m_meshShaders->setUniformMat4("uView", view);

	glm::mat4 model(glm::mat4(1.f));
	model = glm::scale(model, glm::vec3(0.1f));

	glm::mat3 normalMatrix{ glm::transpose(glm::inverse(model)) };

	m_meshShaders->setUniformMat4("uModel", model);
	m_meshShaders->setUniformMat3("uNormalMatrix", normalMatrix);

	m_meshShaders->setUniformVec3("light.direction", glm::vec3(-.2f, -1.f, -2.5f));
	m_meshShaders->setUniformVec3("light.ambient", glm::vec3(.2f));
	m_meshShaders->setUniformVec3("light.diffuse", glm::vec3(.9f));
	m_meshShaders->setUniformVec3("light.specular", glm::vec3(.7f));

	m_meshShaders->setUniformVec3("uViewPos",cam.getPosition());

	m_meshShaders->setUniformf("material.shininess", 32.f);

	m_nanosuit->Draw(m_meshShaders);
}

int main()
{
	Application app{ SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial" };

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