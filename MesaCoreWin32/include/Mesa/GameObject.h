#pragma once
#include "Core.h"

namespace Mesa
{
	class GameObject3D
	{
	public:
		inline void SetColorShader(const uint32_t& shaderId) noexcept { m_RelatedColorShader = shaderId; }
		inline void SetNormalShader(const uint32_t& shaderId) noexcept { m_RelatedNormalShader = shaderId; }
		inline void SetPositionShader(const uint32_t& shaderId) noexcept { m_RelatedPositionShader = shaderId; }
		inline void SetSpecularShader(const uint32_t& shaderId) noexcept { m_RelatedSpecularShader = shaderId; }
		inline void SetModel(const uint32_t& modelId) noexcept { m_RelatedModel = modelId; }

		inline void SetPosition(const glm::vec3& pos) noexcept { m_Position = pos; UpdateWorldMatrix(); }
		inline void SetRotation(const glm::vec3& rot) noexcept { m_Rotation = rot; UpdateWorldMatrix(); }
		inline void SetScale(const glm::vec3& scale) noexcept { m_Scale = scale; UpdateWorldMatrix(); }

		inline glm::mat4x4 GetWorldMatrix() const noexcept { return m_WorldMatrix; }
		inline void SetLayer(const uint32_t& layer) noexcept { m_Layer = layer; }

	private:
		void UpdateWorldMatrix();

	private:
		uint32_t m_RelatedModel = 0; // Model that will represent the game object
		uint32_t m_RelatedColorShader = 0; // Shader that will be used to calculate color data
		uint32_t m_RelatedSpecularShader = 0; // Shader that will be used to calculate specular data
		uint32_t m_RelatedNormalShader = 0; // Shader that will be used to calculate normal data
		uint32_t m_RelatedPositionShader = 0; // Shader that will be used to calculate position data
		uint32_t m_Layer = 0; // Layer that the object will be displayed on

		glm::vec3 m_Position = glm::vec3(0,0,0); // Position of the object in 3D space
		glm::vec3 m_Rotation = glm::vec3(0,0,0); // Rotation of the object in 3D space
		glm::vec3 m_Scale = glm::vec3(1,1,1); // Scale of the object in 3D space

		glm::mat4x4 m_WorldMatrix = glm::mat4x4(1); // World matrix used in MVP calculations
	};
}