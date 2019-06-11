#include "ShaderProgram.h"
#include "ErrorHandler.h"

#include <iostream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram(std::vector<Shader>& shaders)
	: m_shaders(shaders)
{
	m_id = CompileShaders();
}

ShaderProgram::~ShaderProgram()
{
	GL(glDeleteProgram(m_id));
}

GLuint ShaderProgram::CompileShader(Shader shader) {
	
	GLuint id = 0;
	std::string src = shader.getSource();

	const char* cSrc = src.c_str();

	GL((id = glCreateShader(shader.getGLType())));
	GL(glShaderSource(id, 1, &cSrc, NULL));
	GL(glCompileShader(id));

	if (GetStatus(id, Status::SHADER, GL_COMPILE_STATUS, shader.getName(), "COMPILATION FAILURE") != GL_TRUE)
	{
		return 0;
	}

	return id;
}

GLuint ShaderProgram::GetStatus(GLuint id, Status type, GLint statusType, const std::string &errorType, const std::string &errorMsg) {
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

GLuint ShaderProgram::CompileShaders()
{
	GLuint program = glCreateProgram();

	for (auto shader = m_shaders.cbegin(); shader != m_shaders.cend(); shader++)
	{
		GLuint shaderID = CompileShader(*shader);

		GL(glAttachShader(program, shaderID));

		m_activeShaders.push_back(shaderID);
	}

	GL(glLinkProgram(program));

	if (GetStatus(program, Status::PROGRAM, GL_LINK_STATUS, "ShaderProgram", "LINKING FAILURE") != GL_TRUE)
	{
		return 0; // not the best way to do it
	}

	for (auto shader = m_activeShaders.cbegin(); shader != m_activeShaders.cend(); shader++)
	{
		GL(glDeleteShader(*shader));
	}

	return program;
}

void ShaderProgram::Bind() const
{
	GL(glUseProgram(m_id));
}


void ShaderProgram::Unbind() const
{
	GL(glDeleteProgram(m_id));
}

void ShaderProgram::setUniformMat4(const std::string &name, const glm::mat4 &matrix) const {
	GLfloat location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::setUniformMat3(const std::string &name, const glm::mat3 &matrix) const {
	GLfloat location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::setUniform1f(const std::string& name, float value) const
{
	GLfloat location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform1f(location, value));
}

void ShaderProgram::setUniform1i(const std::string& name, float value) const
{
	GLfloat location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform1i(location, value));
}

void ShaderProgram::setUniformVec3(const std::string& name, const glm::vec3 &vector) const
{
	GLfloat location;

	GL((location = glGetUniformLocation(m_id, name.c_str())));

	GL(glUniform3fv(location, 1, glm::value_ptr(vector)));
}
