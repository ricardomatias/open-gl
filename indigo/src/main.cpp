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
#include "FrameBuffer.h"
#include "DepthMap.h"

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
	std::vector<std::shared_ptr<Primitive>> m_primitives;
	std::shared_ptr<ShaderProgram> m_cubeShaders;
	std::shared_ptr<ShaderProgram> m_depthMapShaders;
	std::shared_ptr<ShaderProgram> m_quadShaders;
	std::shared_ptr<ShaderProgram> m_shadowMapShaders;

	std::shared_ptr<Texture> m_cubeTexture;
	std::shared_ptr<Primitive> m_cube;
	std::shared_ptr<DepthMap> m_depthMap;
	glm::mat4 m_lightSpaceMatrix;
public:
	Application(const int windowWidth, const int windowHeight, const char* title)
		: Renderer{windowWidth, windowHeight, title},
			m_cubeShaders(std::make_shared<ShaderProgram>()),
			m_depthMapShaders(std::make_shared<ShaderProgram>()),
			m_quadShaders(std::make_shared<ShaderProgram>()),
			m_shadowMapShaders(std::make_shared<ShaderProgram>()),
			deltaTime(0), currentFrame(0), lastFrame(0), m_lightSpaceMatrix(glm::mat4(1.f))
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
		{ShaderTypes::FRAGMENT, "res/shaders/lighting/directional.frag"}
	};
	
	m_cubeShaders->compileShaders(shaders);

	m_cubeShaders->bind();

	shaders = {
		{ShaderTypes::VERTEX, "res/shaders/basic/depth-map.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/basic/empty.frag"}
	};
	
	m_depthMapShaders->compileShaders(shaders);

	m_depthMapShaders->bind();

	shaders = {
		{ShaderTypes::VERTEX, "res/shaders/basic/quad.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/basic/depth-map.frag"}
	};

	m_quadShaders->compileShaders(shaders);


	m_quadShaders->bind();

	m_quadShaders->setUniformi("depthMap", 0);

	shaders = {
		{ShaderTypes::VERTEX, "res/shaders/basic/shadow-map.vert"},
		{ShaderTypes::FRAGMENT, "res/shaders/basic/shadow-map.frag"}
	};

	m_shadowMapShaders->compileShaders(shaders);

	/** CUBES **/
	m_primitives.emplace_back(std::make_shared<Primitive>(
		Primitive::PLANE,
		glm::vec3(0.f),
		glm::vec3(1.f),
		0.f,
		glm::vec3(2.f)
	));

	m_primitives.emplace_back(std::make_shared<Primitive>(
		Primitive::CUBE,
		glm::vec3(2.0f, 0.0f, 1.0f),
		glm::vec3(1.f),
		0.f,
		glm::vec3(0.5f)
	));

	m_primitives.emplace_back(std::make_shared<Primitive>(
		Primitive::CUBE,
		glm::vec3(0.0f, 1.5f, 0.0f),
		glm::vec3(1.f),
		0.f,
		glm::vec3(0.5f)
	));

	m_primitives.emplace_back(std::make_shared<Primitive>(
		Primitive::CUBE,
		glm::vec3(-1.0f, 0.0f, 2.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		60.f,
		glm::vec3(0.25f)
	));


	/** UNIFORMS **/
	m_cubeTexture = std::make_shared<Texture>("res/images/wood.png", GL_TEXTURE0);

	m_cubeTexture->load();

	m_cubeShaders->bind();

	m_cubeShaders->setUniformi("material.diffuseTex1", 0);

	m_shadowMapShaders->bind();

	m_shadowMapShaders->setUniformi("material.diffuseTex1", 0);
	m_shadowMapShaders->setUniformi("shadowMap", 1);

	/** UNIFORMS **/
	const int SHADOW_WIDTH = 1024;
	const int SHADOW_HEIGHT = 1024;

	m_depthMap = std::make_shared<DepthMap>(SHADOW_WIDTH, SHADOW_HEIGHT);

	float near_plane = 1.0f, far_plane = 7.5f;

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(
		glm::vec3(-2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	m_lightSpaceMatrix = lightProjection * lightView;

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

	/** RENDER SCENE -> DEPTH MAP **/

		/** MATRICES **/
	glm::mat4 view{ cam.getViewMatrix() };
	glm::mat4 proj = glm::perspective(glm::radians(fov), SCR_WIDTH / static_cast<float>(SCR_HEIGHT), .1f, 100.f);

	/** PLANET DRAW **/
	m_depthMapShaders->bind();

	m_depthMapShaders->setUniformMat4("uLightSpace", m_lightSpaceMatrix);

	m_depthMap->prepCapture();

	renderScene(m_depthMapShaders);

	m_depthMap->unbind();

	/** RENDER SCENE -> SHADOW MAPPING **/
	 // reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shadowMapShaders->bind();

	m_cubeTexture->bind();
	m_depthMap->bindDepthMap(GL_TEXTURE1);

	m_shadowMapShaders->setUniformMat4("uProj", proj);
	m_shadowMapShaders->setUniformMat4("uView", view);
	m_shadowMapShaders->setUniformMat4("uLightSpace", m_lightSpaceMatrix);

	m_shadowMapShaders->setUniformVec3("light.position", glm::vec3(-2.0f, 4.0f, -1.0f));
	m_shadowMapShaders->setUniformVec3("light.ambient", glm::vec3(.3f));
	m_shadowMapShaders->setUniformVec3("light.diffuse", glm::vec3(1.f));
	m_shadowMapShaders->setUniformVec3("light.specular", glm::vec3(.1f));

	m_shadowMapShaders->setUniformVec3("uViewPos", cam.getPosition());

	m_shadowMapShaders->setUniformf("material.shininess", 64.f);

	renderScene(m_shadowMapShaders);
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