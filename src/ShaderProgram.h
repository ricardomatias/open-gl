#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <glm\ext\matrix_float4x4.hpp>

#include "Shader.h"

#include "File.h"

typedef std::shared_ptr<Shader> ShaderPointer;

enum class Status
{
	SHADER,
	PROGRAM
};

class ShaderProgram
{
	GLuint m_id;

	std::vector<ShaderPointer> m_shaders;

	void compileShader(ShaderPointer& shader);
public:
	ShaderProgram()
		: m_id(0), m_shaders(std::vector<ShaderPointer>()) {};
	~ShaderProgram() {
		Unbind();

		m_shaders.clear();

		std::cout << "[ShaderProgram] destroyed" << std::endl;
	};

	unsigned int ID() const { return m_id; };

	void compileShaders(std::vector<ShaderPointer> &shaders);

	GLuint GetStatus(GLuint id, Status type, GLint statusType, const std::string& errorType, const std::string& errorMsg);

	void Bind() const;
	void Unbind() const;

	void setUniformf(const std::string& name, float value) const;
	void setUniformi(const std::string& name, int value) const;

	void setUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void setUniformMat4(const std::string& name, const glm::mat4& matrix) const;

	void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
	void setUniformVec3(const std::string& name, float x, float y, float z) const;

	void setUniformArrayVec3(const std::string& name, const std::vector<glm::vec3> vectors) const;
};

