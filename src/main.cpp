#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <array>
#include <filesystem>

#include <cstdio>
#include <cstdlib>

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

namespace fs = std::filesystem;

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

static float lastX = SCR_WIDTH / 2.f, lastY = SCR_HEIGHT / 2.f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

static float fov = 45.f;

Camera cam(glm::vec3(0.f, 2.f, 3.f), glm::vec3(0.f), 2.5f);

struct PointLight
{
	PointLight(glm::vec3 pos) : position(pos) {};

	glm::vec3 position;

	glm::vec3 ambient = glm::vec3(0.2f);
	glm::vec3 diffuse = glm::vec3(0.5f);
	glm::vec3 specular = glm::vec3(1.0f);

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
}; 

std::vector<PointLight> pointLights {
	PointLight{ glm::vec3(0.7f,  0.2f,  2.0f) },
	PointLight{ glm::vec3(2.3f, -3.3f, -4.0f) },
	PointLight{ glm::vec3(-4.0f,  2.0f, -12.0f) },
	PointLight{ glm::vec3(0.0f,  0.0f, -3.0f) },
};

static float deltaTime;

static bool firstMouse = true;

class Application : public Renderer
{
private:
	float currentFrame, lastFrame;
	std::shared_ptr<Model> nanoSuit;
	std::shared_ptr<ShaderProgram> modelShaders;
public:
	Application(int windowWidth, int windowHeight)
		: Renderer{ windowWidth, windowHeight },
		currentFrame(0), lastFrame(0), nanoSuit(std::make_shared<Model>()), modelShaders(std::make_shared<ShaderProgram>())
	{};
	virtual ~Application();

	virtual void setup();
	virtual void draw();
};

Application::~Application()
{
	LOG("Application delete");

	modelShaders->Unbind();
}

void Application::setup()
{
	Shader vert(Shader::VERTEX, std::string("res/shaders/model/basic.vert"));
	Shader frag(Shader::FRAGMENT, std::string("res/shaders/model/basic.frag"));

	std::vector<Shader*> modelShdrs{ &vert, &frag };

	modelShaders->compileShaders( modelShdrs );

	modelShaders->Bind();

	/** UNIFORMS **/
	std::error_code ec;

	auto modelPath = fs::absolute(fs::path("res/models/nanosuit/nanosuit.obj"), ec);

	nanoSuit->loadModel(modelPath.generic_string());
}

void Application::draw()
{
	GLFWwindow* window = getWindow();

	processInput(window);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Simply clear the window with re
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// create transformations
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	view = glm::lookAt(cam.getPosition(), cam.getLookAt(), glm::vec3(0, 1, 0));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.f, 0.f, -4.f));
	model = glm::scale(model, glm::vec3(0.2f));

	/** CUBE DRAW **/
	modelShaders->Bind();

	modelShaders->setUniformMat4("uProj", projection);
	modelShaders->setUniformMat4("uView", view);
	modelShaders->setUniformMat4("uModel", model);

	nanoSuit->Draw(modelShaders);
}


int main()
{
	Application app{ SCR_WIDTH, SCR_HEIGHT };

	app.run();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.move(CameraDirection::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.move(CameraDirection::BACKWARD, deltaTime);
	
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cam.move(CameraDirection::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cam.move(CameraDirection::DOWN, deltaTime);
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.strafe(CameraDirection::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.strafe(CameraDirection::RIGHT, deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	if (state != GLFW_PRESS)
	{
		return;
	}

	cam.lookAround(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= (float)yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}