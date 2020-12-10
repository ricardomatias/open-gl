#pragma once

#include <filesystem>

#include "OpenGL.h"

#include "Renderer.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Primitive.h"

namespace ComputeParticlesSimple
{
	namespace fs = std::filesystem;

	const unsigned int SCR_WIDTH = 780;
	const unsigned int SCR_HEIGHT = 780;

	class Application final : public Renderer
	{
		std::shared_ptr<ShaderProgram> m_shaderProgram;
		std::shared_ptr<ShaderProgram> m_computeProgram;

		std::shared_ptr<Primitive> m_quad;
		GLuint m_posBuf;
		GLuint m_velBuf;
		GLuint m_particlesVAO;

		int m_totalParticles;
		glm::ivec3 m_nParticles;
		GLuint m_blackHoleVAO;
		GLuint m_blackHoleVBO;
		glm::vec4 m_blackHole1;
		glm::vec4 m_blackHole2;
		float m_angle;
		float m_speed;
	public:
		Application(const int windowWidth, const int windowHeight, const char* title)
			: Renderer{windowWidth, windowHeight, title},
			  m_shaderProgram(std::make_shared<ShaderProgram>()),
			  m_computeProgram(std::make_shared<ShaderProgram>()),
			  m_quad(std::make_shared<Primitive>(PrimitiveType::QUAD)), m_posBuf(0), m_velBuf(0), m_particlesVAO(0),
			  m_nParticles(100, 100, 100), m_blackHoleVAO(0), m_blackHoleVBO(0), m_blackHole1(5, 0, 0, 1),
			  m_blackHole2(-5, 0, 0, 1),
			  m_angle(0.f), m_speed(2.f),
			  deltaTime(0), currentFrame(0), lastFrame(0)
		{
			m_totalParticles = m_nParticles.x * m_nParticles.y * m_nParticles.z;
		};

		virtual ~Application();

		virtual void setup();
		virtual void update(double t);
		virtual void draw();

		std::string getPath(const char* path) const;

		std::vector<GLfloat> initPosBuffer();

		double deltaTime;
		double currentFrame, lastFrame;
	};
}
