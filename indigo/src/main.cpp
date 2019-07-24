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
#include "Camera.h"
#include "GUI.h"
#include "ErrorHandler.h"
#include "Model.h"
#include <unordered_map>

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

Camera cam{ glm::vec3(-5.f, 1.f, 40.f), glm::vec3(0.f), 15.f };
GUI gui{ "#version 450" };

glm::vec3 translation = glm::vec3(0.1f);

class Application final : public Renderer
{
	std::shared_ptr<ShaderProgram> m_asteroidShader;
	std::shared_ptr<ShaderProgram> m_planetShader;

	std::shared_ptr<Model> m_planet;
	std::shared_ptr<Model> m_asteroid;
	glm::mat4* m_modelMatrices;
	unsigned int m_asteroidNr;
	GLuint m_asteroidModelBuffer;
public:
	Application(const int windowWidth, const int windowHeight, const char* title)
		: Renderer{windowWidth, windowHeight, title},
			m_asteroidShader(std::make_shared<ShaderProgram>()),
			m_planetShader(std::make_shared<ShaderProgram>()),
			m_planet(std::make_shared<Model>()),
			m_asteroid(std::make_shared<Model>()),
			m_modelMatrices(nullptr),
			m_asteroidNr(0),
			deltaTime(0), currentFrame(0), lastFrame(0)
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
	delete m_modelMatrices;

	LOG("[Application] destroyed");
}

void Application::setup()
{
	// GUI
	//gui.setup(this->getWindow());

	// SHADERS
	std::unordered_map<ShaderTypes, std::string> shaders = {
		{ShaderTypes::VERTEX, "res/shaders/model/basic.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/lighting/directional.frag"}
	};
	
	m_planetShader->compileShaders(shaders);

	shaders = {
		{ShaderTypes::VERTEX, "res/shaders/instance.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/lighting/directional.frag"}
	};

	m_asteroidShader->compileShaders(shaders);
	
	/** nanosuit **/
	std::error_code ec;

	m_planet->loadModel(fs::absolute(fs::path("res/models/planet/planet.obj"), ec).generic_string());

	m_asteroid->loadModel(fs::absolute(fs::path("res/models/rock/rock.obj"), ec).generic_string());

	/** UNIFORMS **/
	m_asteroidNr = 1000;

	m_modelMatrices = new glm::mat4[m_asteroidNr];

	srand(glfwGetTime()); // initialize random seed	

	float radius = 75.0;
	float offset = 20.f;

	for (unsigned int i = 0; i < m_asteroidNr; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = static_cast<float>(i) / m_asteroidNr * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		
		float y = displacement * 0.2f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		m_modelMatrices[i] = model;
	}

	glGenBuffers(1, &m_asteroidModelBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m_asteroidModelBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, m_asteroidNr * sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_asteroidNr * sizeof(glm::mat4), &m_modelMatrices[0]);

	auto asteroids = m_asteroid->getMeshes();

	for (auto &mesh : asteroids)
	{
		GLuint vao = mesh->getVAO();

		glBindVertexArray(vao);

		GLsizei vec4Size = sizeof(glm::vec4); // mat4 = 4 x vec4

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(vec4Size * 0));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(vec4Size * 1));
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(vec4Size * 2));
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, reinterpret_cast<void*>(vec4Size * 3));
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
	
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
	m_planetShader->bind();

	m_planetShader->setUniformMat4("uProj", proj);
	m_planetShader->setUniformMat4("uView", view);

	float rotAngle = 10.f * static_cast<float>(currentFrame);

	glm::mat4 model(glm::mat4(1.f));
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.f, 1.f, 0.f));
	model = glm::rotate(model, glm::radians(-45.f), glm::vec3(0.f, 0.f, 1.f));
	model = glm::scale(model, glm::vec3(4.0f));

	glm::mat3 normalMatrix{ glm::transpose(glm::inverse(model)) };

	m_planetShader->setUniformMat4("uModel", model);
	m_planetShader->setUniformMat3("uNormalMatrix", normalMatrix);

	m_planetShader->setUniformVec3("light.direction", glm::vec3(-.2f, -10.f, -2.5f));
	m_planetShader->setUniformVec3("light.ambient", glm::vec3(.2f));
	m_planetShader->setUniformVec3("light.diffuse", glm::vec3(.9f));
	m_planetShader->setUniformVec3("light.specular", glm::vec3(.7f));

	m_planetShader->setUniformVec3("uViewPos", cam.getPosition());

	m_planetShader->setUniformf("material.shininess", 32.f);

	m_planet->Draw(m_planetShader);

	/** ASTEROIDS DRAW **/
	m_asteroidShader->bind();

	m_asteroidShader->setUniformMat4("uProj", proj);
	m_asteroidShader->setUniformMat4("uView", view);

	m_asteroidShader->setUniformVec3("light.direction", glm::vec3(-.2f, -1.f, -2.5f));
	m_asteroidShader->setUniformVec3("light.ambient", glm::vec3(.2f));
	m_asteroidShader->setUniformVec3("light.diffuse", glm::vec3(.9f));
	m_asteroidShader->setUniformVec3("light.specular", glm::vec3(.7f));

	m_asteroidShader->setUniformVec3("uViewPos", cam.getPosition());

	m_asteroidShader->setUniformf("material.shininess", 32.f);

	auto asteroids = m_asteroid->getMeshes();

	translation += glm::vec3(50.5f);

	for (size_t i = 0; i < asteroids.size(); ++i)
	{
		auto &asteroid = asteroids[i];

		GLsizei vec4size = sizeof(glm::vec4);

		glBindVertexArray(asteroid->getVAO());

		glDrawElementsInstanced(GL_TRIANGLES, asteroid->m_indices.size(), GL_UNSIGNED_INT, nullptr, m_asteroidNr);

		glBindVertexArray(0);
	}

	for (size_t i = 0; i < m_asteroidNr; ++i)
	{
		m_modelMatrices[i] = glm::rotate(m_modelMatrices[i], glm::radians(static_cast<float>(deltaTime)) * 100.f, glm::vec3(0.4f, 0.6f, 0.8f));
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_asteroidModelBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_asteroidNr * sizeof(glm::mat4), nullptr);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_asteroidNr * sizeof(glm::mat4), &m_modelMatrices[0]);
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