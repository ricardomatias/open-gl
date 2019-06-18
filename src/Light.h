#pragma once

#include <glm/glm.hpp>

class Light
{
private:
	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_target;

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;

	float m_pitch;
	float m_yaw;

	void update();
public:
	Light(glm::vec3 pos, glm::vec3 target);

	glm::vec3 getPosition() const { return m_position; };
	glm::vec3 getLookAt() const { return m_front; };

	float getPitch() const { return m_pitch; };
	float getYaw() const { return m_yaw; };

	void pitch(float pitch);
	void yaw(float yaw);
};




