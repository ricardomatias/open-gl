#pragma once

#include <filesystem>

#include <glm/glm.hpp>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Model.h"

namespace InstancingDemo
{
	namespace fs = std::filesystem;

	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 1200;

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
			: Renderer{ windowWidth, windowHeight, title },
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
}
