#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "Camera.h"

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, const float speed = SPEED)
	: m_position(pos), m_target(target)
{
	m_up = WORLD_UP;

	m_front = glm::vec3(0.f, 0.f, -1.f);

	m_direction = glm::normalize(m_position - m_target);
	m_right = WORLD_UP;

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
	front.x = glm::cos(pitch) * glm::cos(yaw);
	front.y = glm::sin(pitch);
	front.z = glm::cos(pitch) * glm::sin(yaw);

	m_front = glm::normalize(front);

	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::move(CameraDirection direction, float deltaTime)
{
	float speed = m_speed * deltaTime;

	switch (direction)
	{
		case CameraDirection::FORWARD:
			m_position += speed * m_front;
			break;
		case CameraDirection::BACKWARD:
			m_position -= speed * m_front;
			break;
		case CameraDirection::UP:
			m_position += speed * m_up;
			break;
		case CameraDirection::DOWN:
			m_position -= speed * m_up;
			break;
		default:
			break;
	}
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

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(getPosition(), getLookAt(), WORLD_UP);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Camera::keyboardListen(GLFWwindow* window, const double deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		this->move(CameraDirection::FORWARD, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		this->move(CameraDirection::BACKWARD, static_cast<float>(deltaTime));

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		this->move(CameraDirection::UP, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		this->move(CameraDirection::DOWN, static_cast<float>(deltaTime));

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->strafe(CameraDirection::LEFT, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->strafe(CameraDirection::RIGHT, static_cast<float>(deltaTime));
}