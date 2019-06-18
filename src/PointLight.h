#pragma once

#include <glm/glm.hpp>

class Light
{
private:
	glm::vec3 m_position;
	glm::vec3 m_target;

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_left;
public:
	Light();
	~Light();


};

Light::Light()
{

}

Light::~Light()
{
}