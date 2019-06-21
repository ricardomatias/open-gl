#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

#include "Indigo.h"

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

	unsigned int m_cubeVao, m_cubeVbo;
	unsigned int m_planeVao, m_planeVbo;

	std::shared_ptr<Texture> m_cubeTexture;
	std::shared_ptr<Texture> m_planeTexture;
	std::shared_ptr<Cubemap> m_cubemap;
public:
	Application(const int windowWidth, const int windowHeight, const char* title)
		: Renderer{ windowWidth, windowHeight, title },
		m_meshShaders(std::make_shared<ShaderProgram>()),
		m_cubeVao(0), m_cubeVbo(0), m_planeVao(0), m_planeVbo(0),
		m_cubemap(std::make_shared<Cubemap>()),
		deltaTime(0),
		currentFrame(0), lastFrame(0)
	{};
	virtual ~Application();

	virtual void setup();
	virtual void draw();

	void drawCubes(std::shared_ptr<ShaderProgram>& shader, bool shouldScale = false);

	double deltaTime;
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
	auto frag = std::make_shared<Shader>(Shader::FRAGMENT, std::string("res/shaders/basic/texture.frag"));

	std::vector<std::shared_ptr<Shader>> shaders{ std::move(vert), std::move(frag) };

	m_meshShaders->compileShaders(shaders);

	/** CUBE **/
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

	/** PLANE **/
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

	/** SKYBOX **/
	m_cubemap->createCube();

	/** UNIFORMS **/
	std::error_code ec;

	const std::string texPath = fs::absolute(fs::path("res/images"), ec).generic_string();

	std::vector<std::string> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	m_cubemap->load(texPath + "/skybox/", faces);

	const std::string cubeTexPath = texPath + std::string("/container.jpg");
	const std::string planeTexPath = texPath + std::string("/metal.png");

	m_cubeTexture = std::make_shared<Texture>(cubeTexPath, GL_TEXTURE0);
	m_planeTexture = std::make_shared<Texture>(planeTexPath, GL_TEXTURE0);

	m_cubeTexture->load();
	m_planeTexture->load();

	m_meshShaders->bind();
	m_meshShaders->setUniformi("tex0", 0);

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
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_meshShaders->bind();

	const glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

	glm::mat4 view = cam.getViewMatrix();

	m_meshShaders->setUniformMat4("uProj", projection);
	m_meshShaders->setUniformMat4("uView", view);

	/** FLOOR DRAW **/
	glBindVertexArray(m_planeVao);
	m_planeTexture->Bind();

	m_meshShaders->setUniformMat4("uModel", glm::mat4(1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	/** CUBE DRAW **/
	drawCubes(m_meshShaders);

	/** SKYBOX DRAW **/
	m_cubemap->draw(projection, view);
}

void Application::drawCubes(std::shared_ptr<ShaderProgram>& shader, bool shouldScale)
{
	glm::mat4 model{ 1.f };

	shader->bind();

	glBindVertexArray(m_cubeVao);

	m_cubeTexture->Bind();

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