#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// Default camera values
static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 2.5f;
static const glm::vec3 WORLD_UP(0, 1, 0);


enum struct CameraDirection
{
	FORWARD = 0,
	BACKWARD = 1,
	LEFT = 2,
	RIGHT = 3
};

class Camera
{
private:
	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_direction; // z - axis

	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;

	float m_speed;

	float m_pitch;
	float m_yaw;

	void update();
public:
	Camera(const glm::vec3& pos, const glm::vec3& target, float speed);

	glm::vec3 getPosition() const { return m_position; };
	glm::vec3 getLookAt() const { return m_position + m_front; };
	glm::vec3 getTarget() const { return m_target; };

	float getSpeed() const { return m_speed; };

	void move(CameraDirection direction, float deltaTime);
	void strafe(CameraDirection direction, float deltaTime);
	void lookAround(float xoff, float yoff);
};

