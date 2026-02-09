#include <Mesa/GameObject.h>

namespace Mesa
{
	/*
		Recalculates world matrix.
	*/
	void GameObject3D::UpdateWorldMatrix()
	{
		m_WorldMatrix = glm::mat4x4(1.0f);

		m_WorldMatrix = glm::rotate(m_WorldMatrix, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_WorldMatrix = glm::rotate(m_WorldMatrix, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_WorldMatrix = glm::rotate(m_WorldMatrix, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_WorldMatrix = glm::translate(m_WorldMatrix, m_Position);

		m_WorldMatrix = glm::scale(m_WorldMatrix, m_Scale);
	}
}