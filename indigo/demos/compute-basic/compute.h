#pragma once

#include <filesystem>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Primitive.h"

namespace BasicComputeDemo
{
	namespace fs = std::filesystem;

	const unsigned int SCR_WIDTH = 1024;
	const unsigned int SCR_HEIGHT = 1024;

	class Application final : public Renderer
	{
		std::shared_ptr<ShaderProgram> m_shaderProgram;
		std::shared_ptr<ShaderProgram> m_computeProgram;

		std::shared_ptr<Texture> m_imageTex;
		std::shared_ptr<Texture> m_computedTex;
		std::shared_ptr<Primitive> m_quad;
	public:
		Application(const int windowWidth, const int windowHeight, const char* title)
			: Renderer{ windowWidth, windowHeight, title },
			m_shaderProgram(std::make_shared<ShaderProgram>()),
			m_computeProgram(std::make_shared<ShaderProgram>()),
			m_imageTex(std::make_shared<Texture>()),
			m_computedTex(std::make_shared<Texture>()),
			m_quad(std::make_shared<Primitive>(PrimitiveType::QUAD)),
			deltaTime(0), currentFrame(0), lastFrame(0)
		{};
		virtual ~Application();

		virtual void setup();
		virtual void draw();

		double deltaTime;
		double currentFrame, lastFrame;
	};
}
