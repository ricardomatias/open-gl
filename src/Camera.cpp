#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, const float speed = SPEED)
	: m_position(pos), m_target(target)
{
	m_up = WORLD_UP;

	m_front = glm::vec3(0, 0, -1);

	m_direction = glm::normalize(m_position - m_target);
	m_right = glm::normalize(glm::cross(m_up, m_direction));

	m_speed = speed;

	m_yaw = YAW;
	m_pitch = PITCH;
}

void Camera::update()
{
	glm::vec3 front;

	float pitch = glm::radians(m_pitch);
	float yaw = glm::radians(m_yaw);

	// pitch -> angle between the plane ZX and Y
	// yaw -> angle between X and Z
	front.x = cos(pitch) * cos(yaw);
	front.y = sin(pitch);
	front.z = cos(pitch) * sin(yaw);

	m_front = glm::normalize(front);

	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::move(CameraDirection direction, float deltaTime)
{
	glm::vec3 displacement = m_speed * deltaTime * m_front;

	if (direction == CameraDirection::FORWARD) m_position += displacement;
	if (direction == CameraDirection::BACKWARD) m_position -= displacement;
}

void Camera::strafe(CameraDirection direction, float deltaTime)
{
	glm::vec3 dir = glm::normalize(glm::cross(m_front, m_up));

	float speed = m_speed * deltaTime;

	if (direction == CameraDirection::LEFT) m_position -= dir * speed;
	if (direction == CameraDirection::RIGHT) m_position += dir * speed;
}

void Camera::lookAround(float xoff, float yoff)
{
	m_pitch = glm::max(glm::min(m_pitch + yoff, 89.f), -89.f);
	m_yaw = glm::mod(m_yaw + xoff, 360.f);
	
	update();
}