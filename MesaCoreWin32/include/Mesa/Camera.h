#pragma once
#include "Core.h"

namespace Mesa
{
    enum CameraMovement {
        CameraMovementForward,
        CameraMovementBackward,
        CameraMovementLeft,
        CameraMovementRight,
        CameraMovementUp,
        CameraMovementDown
    };

	class MSAPI Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
        
        void SetProjectionValues(uint32_t width, uint32_t height, float nearZ, float farZ);
        void ProcessMovement(CameraMovement direction, float deltaTime = 1.0f);
        void ProcessRotation(float xoffset, float yoffset, bool constrainPitch = true);

    private:
        void UpdateCameraVectors();
        void UpdateViewMatrix();

	private:
        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;
        
        float m_Yaw;
        float m_Pitch;

        float m_MovementSpeed;
        float m_MouseSensitivity;
        float m_Zoom;

        glm::mat4x4 m_Projection;
        glm::mat4x4 m_View;
	};
}