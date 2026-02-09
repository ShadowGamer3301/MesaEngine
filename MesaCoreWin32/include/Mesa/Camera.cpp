#include "Camera.h"

Mesa::Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	m_Position = position;
	m_WorldUp = up;
	m_Yaw = yaw;
	m_Pitch = pitch;
	m_MovementSpeed = 2.5f;
	m_MouseSensitivity = 0.1f;
	m_Zoom = 45.0f;
	UpdateCameraVectors();
	UpdateViewMatrix();
}

void Mesa::Camera::SetProjectionValues(uint32_t width, uint32_t height, float nearZ, float farZ)
{
	m_Projection = glm::perspective(glm::radians(m_Zoom), (float)width / (float)height, 0.1f, 100.0f);
}

void Mesa::Camera::ProcessMovement(CameraMovement direction, float deltaTime)
{
	float velocity = m_MovementSpeed * deltaTime;
	if (direction == CameraMovementForward)
		m_Position += m_Front * velocity;
	if (direction == CameraMovementBackward)
		m_Position -= m_Front * velocity;
	if (direction == CameraMovementLeft)
		m_Position -= m_Right * velocity;
	if (direction == CameraMovementRight)
		m_Position += m_Right * velocity;
	if (direction == CameraMovementUp)
		m_Position += m_Up * velocity;
	if (direction == CameraMovementDown)
		m_Position -= m_Up * velocity;

	UpdateViewMatrix();
}

void Mesa::Camera::ProcessRotation(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}

	UpdateCameraVectors();
	UpdateViewMatrix();
}

void Mesa::Camera::UpdateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(front);
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Mesa::Camera::UpdateViewMatrix()
{
	m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}
