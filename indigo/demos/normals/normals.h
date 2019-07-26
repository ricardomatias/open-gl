#pragma once

#include <vector>
#include <filesystem>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Primitive.h"

namespace NormalsDemo
{
	namespace fs = std::filesystem;

	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 1200;

	class Application final : public Renderer
	{
		std::vector<std::shared_ptr<Primitive>> m_primitives;
		std::shared_ptr<ShaderProgram> m_modelShaders;

		std::shared_ptr<Model> m_nanoSuit;
	public:
		Application(const int windowWidth, const int windowHeight, const char* title)
			: Renderer{ windowWidth, windowHeight, title },
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
}

