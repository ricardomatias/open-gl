#include "Light.h"

static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const glm::vec3 WORLD_UP(0, 1, 0);

Light::Light(glm::vec3 pos, glm::vec3 target)
	: m_position(pos), m_target(target)
{
	m_front = glm::vec3(0.f, 0.f, -1.f);
	m_direction = glm::normalize(pos - target);

	m_right = glm::cross(WORLD_UP, m_direction);
	m_up = glm::cross(m_right, m_direction);

	m_yaw = YAW;
	m_pitch = PITCH;
}

void Light::update()
{
	glm::vec3 front;

	float pitch = glm::radians(m_pitch);
	float yaw = glm::radians(m_yaw);

	front.x = glm::cos(pitch) * glm::cos(yaw);
	front.y = glm::sin(pitch);
	front.z = glm::cos(pitch) * glm::sin(yaw);

	m_front = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Light::pitch(float pitch)
{
	m_pitch = glm::max(glm::min(pitch, 89.f), -89.f);

	update();
}

void Light::yaw(float yaw)
{
	m_yaw = glm::max(glm::min(yaw, 360.f), -360.f);

	update();
}
