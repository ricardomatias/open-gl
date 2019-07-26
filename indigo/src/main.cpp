#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GUI.h"
#include "ErrorHandler.h"
#include "Model.h"
#include "Primitive.h"

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

glm::vec3 modelPosition(0.f, 0.f, -5.f);

Camera cam{ glm::vec3(0.0f, 0.0f, 3.0f), modelPosition, 5.f };
GUI gui{ "#version 450" };

class Application final : public Renderer
{
	std::vector<std::shared_ptr<Primitive>> m_primitives;
	std::shared_ptr<ShaderProgram> m_modelShaders;

	std::shared_ptr<Model> m_nanoSuit;
public:
	Application(const int windowWidth, const int windowHeight, const char* title)
		: Renderer{windowWidth, windowHeight, title},
			m_modelShaders(std::make_shared<ShaderProgram>()),
			m_nanoSuit(std::make_shared<Model>()),
			deltaTime(0), currentFrame(0), lastFrame(0)
	{};
	virtual ~Application();

	virtual void setup();
	virtual void draw();
	void renderScene(std::shared_ptr<ShaderProgram>& shader);

	double deltaTime;
	double currentFrame, lastFrame;
};

Application::~Application()
{
	//gui.destroy();
	LOG("[Application] destroyed");
}

void Application::setup()
{
	// GUI
	//gui.setup(this->getWindow());

	// SHADERS
	std::unordered_map<ShaderTypes, std::string> shaders = {
		{ShaderTypes::VERTEX, "res/shaders/model/basic.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/lighting/point.frag"}
	};
	
	m_modelShaders->compileShaders(shaders);

	m_modelShaders->bind();

	/** MODEL **/
	std::error_code ec;

	fs::path modelPath = fs::absolute(fs::path("res/models/nanosuit/nanosuit.obj"), ec);

	m_nanoSuit->loadModel(modelPath.generic_string());

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
	glm::mat4 proj = glm::perspective(glm::radians(fov), SCR_WIDTH / static_cast<float>(SCR_HEIGHT), .1f, 100.f);

	/** RENDER SCENE -> SHADOW MAPPING **/
	m_modelShaders->bind();

	m_modelShaders->setUniformMat4("uProj", proj);
	m_modelShaders->setUniformMat4("uView", view);

	m_modelShaders->setUniformVec3("light.ambient", glm::vec3(.3f));
	m_modelShaders->setUniformVec3("light.diffuse", glm::vec3(1.f));
	m_modelShaders->setUniformVec3("light.specular", glm::vec3(.75f));

	m_modelShaders->setUniformf("light.constant", 1.f);
	m_modelShaders->setUniformf("light.linear", .014f);
	m_modelShaders->setUniformf("light.quadratic", 0.0007f);

	m_modelShaders->setUniformVec3("uLightPos", glm::vec3(0.f, 1.f, -1.f));
	m_modelShaders->setUniformVec3("uViewPos", cam.getPosition());

	m_modelShaders->setUniformf("material.shininess", 32.f);

	glm::mat4 model(glm::mat4(1.f));
	model = glm::translate(model, modelPosition);
	model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
	model = glm::scale(model, glm::vec3(0.2f));

	glm::mat3 normalMatrix{ glm::transpose(glm::inverse(model)) };

	m_modelShaders->setUniformMat4("uModel", model);
	m_modelShaders->setUniformMat3("uNormalMatrix", normalMatrix);

	m_nanoSuit->Draw(m_modelShaders);
}

void Application::renderScene(std::shared_ptr<ShaderProgram> &shader)
{
	for (auto& primitive : m_primitives)
	{
		glm::mat4 model(glm::mat4(1.f));
		model = glm::translate(model, primitive->position);
		model = glm::rotate(model, glm::radians(primitive->rotAngle), primitive->rotVec);
		model = glm::scale(model, primitive->scale);

		glm::mat3 normalMatrix{ glm::transpose(glm::inverse(model)) };

		shader->setUniformMat4("uModel", model);
		shader->setUniformMat3("uNormalMatrix", normalMatrix);

		primitive->draw();
	}
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