#pragma once

#include <vector>
#include <string>
#include <glm\ext\matrix_float4x4.hpp>

#include "Shader.h"

#include "File.h"

enum class Status
{
	SHADER,
	PROGRAM
};

class ShaderProgram
{
private:
	unsigned int m_id;

	std::vector<Shader> m_shaders;
	std::vector<int> m_activeShaders;
public:

	ShaderProgram(std::vector<Shader> &shaders);
	~ShaderProgram();

	unsigned int ID() const { return m_id; };

	GLuint CompileShaders();
	GLuint CompileShader(Shader shader);

	GLuint GetStatus(GLuint id, Status type, GLint statusType, const std::string& errorType, const std::string& errorMsg);

	void Bind() const;
	void Unbind() const;

	void setUniform1f(const std::string& name, float value) const;
	void setUniform1i(const std::string& name, float value) const;

	void setUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void setUniformMat4(const std::string& name, const glm::mat4& matrix) const;

	void setUniformVec3(const std::string& name, const glm::vec3& vector) const;
};