#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

#include <cstdio>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ErrorHandler.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Cube.h"


// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

static float lastX = SCR_WIDTH / 2.f, lastY = SCR_HEIGHT / 2.f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

static float fov = 45.f;

Camera cam(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), 2.5f);

glm::vec3 lightPos(0.0f, .75f, 0.0f);

static float deltaTime;

static bool firstMouse = true;

int main(void) {
	GLFWwindow* window;

	const char* glsl_version = "#version 450";

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // V-SYNC

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	fprintf(stdout, "Status: Usindg GLEW %s\n", glewGetString(GLEW_VERSION));

	std::cout << glGetString(GL_VERSION) << std::endl;

	/** IMGUI **/
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetStyle().ScaleAllSizes(1.5f);
	ImGui::GetIO().FontGlobalScale = 1.5f;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	
	// Setup Dear ImGui sty
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	/* OPEN-GL DEFAULTS */
	glEnable(GL_DEPTH_TEST);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/** BEGIN CLASS SCOPE**/
	{

		Shader vert(Shader::VERTEX, std::string("res/shaders/lighting/light.vert"));
		Shader frag(Shader::FRAGMENT, std::string("res/shaders/lighting/light.frag"));
		Shader lampFrag(Shader::FRAGMENT, std::string("res/shaders/lighting/lamp.frag"));

		std::vector<Shader> cubeShdrs { vert, frag };
		std::vector<Shader> lampShdrs { vert, lampFrag };

		ShaderProgram lightingShaders(cubeShdrs);
		ShaderProgram lampShaders(lampShdrs);

		//glm::vec3 cubePositions[] = {
		//	glm::vec3(0.0f,  0.0f,  0.0f),
		//	glm::vec3(2.0f,  5.0f, -15.0f),
		//	glm::vec3(-1.5f, -2.2f, -2.5f),
		//	glm::vec3(-3.8f, -2.0f, -12.3f),
		//	glm::vec3(2.4f, -0.4f, -3.5f),
		//	glm::vec3(-1.7f,  3.0f, -7.5f),
		//	glm::vec3(1.3f, -2.0f, -2.5f),
		//	glm::vec3(1.5f,  2.0f, -2.5f),
		//	glm::vec3(1.5f,  0.2f, -1.5f),
		//	glm::vec3(-1.3f,  1.0f, -1.5f)
		//};


		GLuint cubeVAO;

		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);

		GLuint cubeBuffer;

		glGenBuffers(1, &cubeBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE), CUBE, GL_STATIC_DRAW);

		GLint posAttrib;

		GL((posAttrib = glGetAttribLocation(lightingShaders.ID(), "aPos")));
		GL(glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), nullptr));
		GL(glEnableVertexAttribArray(posAttrib));

		GLint normalAttrib;

		GL((normalAttrib = glGetAttribLocation(lightingShaders.ID(), "aNormal")));
		GL(glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(5 * sizeof(GL_FLOAT))));
		GL(glEnableVertexAttribArray(normalAttrib));

		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		GLuint lightVAO;

		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);

		GL((posAttrib = glGetAttribLocation(lightingShaders.ID(), "aPos")));
		GL(glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), nullptr));
		GL(glEnableVertexAttribArray(posAttrib));

		/** UNIFORMS **/
		auto t_start = std::chrono::high_resolution_clock::now();

		glm::mat4 model = glm::mat4(1.0f);

		float currentFrame, lastFrame = 0;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			// input
			processInput(window);
			glfwSetScrollCallback(window, scrollCallback);
			glfwSetCursorPosCallback(window, mouseCallback);

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			currentFrame = (float)glfwGetTime();
			
			deltaTime = currentFrame - lastFrame;

			lastFrame = currentFrame;

			//{
			//	ImGui::Begin("Camera / View");

			//	ImGui::SliderFloat("Camera Speed", &cameraSpeed, 2.5f, 5.f);

			//	ImGui::End();
			//}

			/* Render here */
			auto t_now = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

			// Simply clear the window with re
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// create transformations
			glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			view = glm::lookAt(cam.getPosition(), cam.getLookAt(), glm::vec3(0, 1, 0));

			/** CUBE DRAW **/
			model = glm::mat4(1.0f);

			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model))); // we need world space vertex coordinates for the lighting

			lightingShaders.Bind();
			lightingShaders.setUniformMat4("u_Proj", projection);
			lightingShaders.setUniformMat4("u_View", view);
			lightingShaders.setUniformMat4("u_Model", model);
			lightingShaders.setUniformMat3("u_NormalMatrix", normalMatrix);

			lightingShaders.setUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
			lightingShaders.setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
			lightingShaders.setUniformVec3("lightPos", lightPos);
			lightingShaders.setUniformVec3("camPos", cam.getPosition());

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//for (unsigned int i = 0; i < 10; i++)
			//{
			//	model = glm::mat4(1.0f);

			//	model = glm::translate(model, cubePositions[i]);

			//	float angle = 20.0f * i;

			//	model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			//}

			/** LAMP DRAW **/
			float radius = 1.5f;

			lightPos.x = glm::cos(time * 1.2) * radius;
			lightPos.z = glm::sin(time) * radius;

			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f));

			lampShaders.Bind();

			lampShaders.setUniformMat4("u_Proj", projection);
			lampShaders.setUniformMat4("u_View", view);
			lampShaders.setUniformMat4("u_Model", model);

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		/** END CLASS SCOPE**/
		 // optional: de-allocate all resources once they've outlived their purpose:
		// ------------------------------------------------------------------------
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &lightVAO);
		glDeleteBuffers(1, &cubeBuffer);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
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
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}