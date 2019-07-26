#pragma once

#include <filesystem>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"

namespace DemoTemplate
{
	namespace fs = std::filesystem;

	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 1200;

	class Application final : public Renderer
	{
		std::shared_ptr<ShaderProgram> m_shaderProgram;
	public:
		Application(const int windowWidth, const int windowHeight, const char* title)
			: Renderer{ windowWidth, windowHeight, title },
			m_shaderProgram(std::make_shared<ShaderProgram>()),
			deltaTime(0), currentFrame(0), lastFrame(0)
		{};
		virtual ~Application();

		virtual void setup();
		virtual void draw();

		double deltaTime;
		double currentFrame, lastFrame;
	};
}
