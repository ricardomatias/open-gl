#pragma once

#include <GL/glew.h>
#include "File.h"

static enum class ShaderTypes
{
	VERTEX,
	TESS_CTRL,
	TESS_EVAL,
	GEOMETRY,
	FRAGMENT,
};

struct Shader
{
private:
	ShaderTypes m_type;
	std::string m_path;
	std::string m_src;
	int m_GLType;
	std::string m_name;
public:
	Shader(const ShaderTypes type, const std::string& path)
	{
		m_type = type;
		m_path = path;
		m_src = Read(path);

		switch (type)
		{
			case ShaderTypes::VERTEX:
				m_GLType = GL_VERTEX_SHADER;
				m_name = "Vertex";
				break;
			case ShaderTypes::TESS_CTRL:
				m_GLType = GL_TESS_CONTROL_SHADER;
				m_name = "Tesselation Control";
				break;
			case ShaderTypes::TESS_EVAL:
				m_GLType = GL_TESS_EVALUATION_SHADER;
				m_name = "Tesselation Evaluation";
				break;
			case ShaderTypes::GEOMETRY:
				m_GLType = GL_GEOMETRY_SHADER;
				m_name = "Geometry";
				break;
			case ShaderTypes::FRAGMENT:
				m_GLType = GL_FRAGMENT_SHADER;
				m_name = "Fragment";
				break;
			default:
				break;
		}
	};

	ShaderTypes getType() const { return m_type; };
	std::string getPath() const { return m_path; };
	std::string getSource() const { return m_src; };
	std::string getName() const { return m_name; };
	int getGLType() const { return m_GLType; };

	static const ShaderTypes VERTEX = ShaderTypes::VERTEX;
	static const ShaderTypes TESS_CTRL = ShaderTypes::TESS_CTRL;
	static const ShaderTypes TESS_EVAL = ShaderTypes::TESS_EVAL;
	static const ShaderTypes GEOMETRY = ShaderTypes::GEOMETRY;
	static const ShaderTypes FRAGMENT = ShaderTypes::FRAGMENT;
};