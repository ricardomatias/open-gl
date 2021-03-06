#include "ShaderProgram.h"
#include "ErrorHandler.h"

#include <iostream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>


void ShaderProgram::logActiveAttributes()
{
	GLint numAttribs;

	GL(glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	std::cout << "Active attributes" << std::endl;

	for (int i = 0; i < numAttribs; ++i)
	{
		GLint results[3];
		glGetProgramResourceiv(m_id, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, results);

		GLint nameBufSize = results[0] + 1;
		char* name = new char[nameBufSize];

		glGetProgramResourceName(m_id, GL_PROGRAM_INPUT, i, nameBufSize, nullptr, name);

		printf("%-5d %s (%s)n", results[2], name, getTypeString(results[1]));

		delete[] name;
	}
};

GLuint ShaderProgram::GetStatus(GLuint id, Status type, GLint statusType, const std::string& errorType,
                                const std::string& errorMsg)
{
	GLint success = 0;

	if (type == Status::SHADER)
	{
		GL(glGetShaderiv(id, statusType, &success)); // get the compilation result: 1 passed, 0 failed
	}

	if (type == Status::PROGRAM)
	{
		GL(glGetProgramiv(id, statusType, &success)); // get the compilation result: 1 passed, 0 failed
	}

	if (success != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024]; // hack due to not knowing the length ahead of time || FIND ALTERNATIVE

		if (type == Status::SHADER)
		{
			glGetShaderInfoLog(id, 1024, &logLength, message);
		}

		if (type == Status::PROGRAM)
		{
			glGetProgramInfoLog(id, 1024, &logLength, message);
		}

		std::cout << "[" << errorType << "]: " << errorMsg << std::endl;

		std::cout << message << std::endl;

		glDeleteShader(id);

		return 0;
	}

	return GL_TRUE;
}

void ShaderProgram::compileShader(ShaderPointer& shader)
{
	GLuint id;
	const std::string src = shader->getSource();

	const char* cSrc = src.c_str();

	GL((id = glCreateShader(shader->getGLType())));
	GL(glShaderSource(id, 1, &cSrc, NULL));
	GL(glCompileShader(id));

	if (GetStatus(id, Status::SHADER, GL_COMPILE_STATUS, shader->getName(), "COMPILATION FAILURE") != GL_TRUE)
	{
		return;
	}

	shader->setID(id);
}

void ShaderProgram::createShaders(const std::unordered_map<ShaderTypes, std::string>& shadersConf)
{
	for (const auto& shaderIter : shadersConf)
	{
		auto shader = std::make_shared<Shader>(shaderIter.first, shaderIter.second);

		m_shaders.emplace_back(std::move(shader));
	}
}

void ShaderProgram::compileShaders(std::unordered_map<ShaderTypes, std::string>& shadersConf)
{
	createShaders(shadersConf);

	const GLuint program = glCreateProgram();

	for (auto& shaderIter : m_shaders)
	{
		ShaderPointer& shader = shaderIter;

		compileShader(shaderIter);

		GL(glAttachShader(program, shader->ID()));
	}

	GL(glLinkProgram(program));

	if (GetStatus(program, Status::PROGRAM, GL_LINK_STATUS, "ShaderProgram", "LINKING FAILURE") != GL_TRUE)
	{
		return; // not the best way to do it
	}

	for (const auto& shader : m_shaders)
	{
		GL(glDeleteShader(shader->ID()));
	}

	m_id = program;
}

void ShaderProgram::bind() const
{
	GL(glUseProgram(m_id));
}


void ShaderProgram::unbind() const
{
	GL(glUseProgram(0));
}

void ShaderProgram::destroy() const
{
	unbind();

	GL(glDeleteProgram(m_id));
}

void ShaderProgram::setUniformMat4(const std::string& name, glm::mat4& matrix) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::setUniformMat3(const std::string& name, glm::mat3& matrix) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::setUniformf(const std::string& name, float value) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform1f(location, value));
}

void ShaderProgram::setUniformi(const std::string& name, int value) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform1i(location, value));
}

void ShaderProgram::setUniformVec3(const std::string& name, const glm::vec3& vector) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform3fv(location, 1, glm::value_ptr(vector)));
}

void ShaderProgram::setUniformArrayVec3(const std::string& name, const std::vector<glm::vec3> vectors) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform3fv(location, (GLsizei)vectors.size(), glm::value_ptr(vectors[0])));
}

void ShaderProgram::setUniformVec3(const std::string& name, float x, float y, float z) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform3f(location, x, y, z));
}

void ShaderProgram::setUniformVec4(const std::string& name, const glm::vec4& vector) const
{
	GLint location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform4fv(location, 1, glm::value_ptr(vector)));
}

const char* ShaderProgram::getTypeString(GLenum type) {
	switch (type)
	{
		case GL_FLOAT:
			return "float";
		case GL_FLOAT_VEC2:
			return "vec2";
		case GL_FLOAT_VEC3:
			return "vec3";
		case GL_FLOAT_VEC4:
			return "vec4";
		case GL_DOUBLE:
			return "double";
		case GL_INT:
			return "int";
		case GL_UNSIGNED_INT:
			return "unsigned int";
		case GL_BOOL:
			return "bool";
		case GL_FLOAT_MAT2:
			return "mat2";
		case GL_FLOAT_MAT3:
			return "mat3";
		case GL_FLOAT_MAT4:
			return "mat4";
		default:
			return "?";
	}
}