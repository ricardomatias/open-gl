#pragma once

#include "OpenGL.h"
#include <iostream>
#include "File.h"


enum class ShaderTypes
{
	VERTEX,
	TESS_CTRL,
	TESS_EVAL,
	GEOMETRY,
	FRAGMENT,
};

class Shader
{
	GLuint m_id;
	ShaderTypes m_type;
	std::string m_path;
	std::string m_src;
	int m_glType;
	std::string m_name;
public:
	Shader(const ShaderTypes type, const std::string& path) : m_id(0), m_type(type), m_path(path)
	{
		m_src = Read(path);

		switch (type)
		{
			case ShaderTypes::VERTEX:
				m_glType = GL_VERTEX_SHADER;
				m_name = "Vertex";
				break;
			case ShaderTypes::TESS_CTRL:
				m_glType = GL_TESS_CONTROL_SHADER;
				m_name = "Tesselation Control";
				break;
			case ShaderTypes::TESS_EVAL:
				m_glType = GL_TESS_EVALUATION_SHADER;
				m_name = "Tesselation Evaluation";
				break;
			case ShaderTypes::GEOMETRY:
				m_glType = GL_GEOMETRY_SHADER;
				m_name = "Geometry";
				break;
			case ShaderTypes::FRAGMENT:
				m_glType = GL_FRAGMENT_SHADER;
				m_name = "Fragment";
				break;
			default:
				break;
		}
	};

	~Shader() {
		std::cout << "[Shader] destroyed" << std::endl;
	};

	GLuint ID() const { return m_id; };
	void setID(GLuint id) { m_id = id; };

	ShaderTypes getType() const { return m_type; };
	std::string getPath() const { return m_path; };
	std::string getSource() const { return m_src; };
	std::string getName() const { return m_name; };
	int getGLType() const { return m_glType; };
};

