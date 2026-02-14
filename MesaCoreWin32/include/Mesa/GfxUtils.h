#pragma once
#include "Core.h"

namespace Mesa
{
	/*
		Vertex structure used for forward rendering
	*/
	struct ForwardVertexDx11
	{
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT2 m_TexCoord;
		DirectX::XMFLOAT3 m_Normal;
	};

	/*
		Vertex structure used for deferred rendering
	*/
	struct DeferredVertexDx11
	{
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT2 m_TexCoord;
	};

	enum ShaderType
	{
		ShaderType_Forward = 0, // Used for forward rendering
		ShaderType_Deferred = 1, // Used for deferred rendering
	};

	struct VertexDx11
	{
		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT2 m_TexCoord;
		DirectX::XMFLOAT3 m_Normal;
	};

	class MSAPI Shader
	{
	public:
		inline uint32_t GetShaderUID() const noexcept { return m_ShaderUID; }
		inline ShaderType GetShaderType() const noexcept { return m_ShaderType; }
		inline std::string GetVertexShaderName() const noexcept { return m_VertexShaderName; }
		inline std::string GetPixelShaderName() const noexcept { return m_PixelShaderName; }

	protected:
		std::string m_VertexShaderName = std::string();
		std::string m_PixelShaderName = std::string();
		uint32_t m_ShaderUID = 0;
		ShaderType m_ShaderType = ShaderType_Forward;
	};

	class MSAPI Model
	{
	public:
		inline uint32_t GetModelUID() const noexcept { return m_ModelUID; }
		inline std::string GetModelName() const noexcept { return m_ModelName; }

	protected:
		uint32_t m_ModelUID = 0;
		std::string m_ModelName = std::string();
	};

	class MSAPI Texture
	{
	public:
		inline uint32_t GetTextureUID() const noexcept { return m_TextureUID; }
		inline std::string GetTextureName() const noexcept { return m_TextureName; }

	protected:
		std::string m_TextureName = std::string();
		uint32_t m_TextureUID = 0;
	};

	class MSAPI ShaderDx11 : public Shader
	{
		friend class GraphicsDx11;
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mp_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mp_InputLayout;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mp_PixelShader;
	};

	class MSAPI TextureDx11 : public Texture
	{
		friend class GraphicsDx11;
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mp_RawData;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mp_ResourceView;
	};

	class MSAPI MeshDx11
	{
		friend class GraphicsDx11;
		friend class ModelDx11;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mp_VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mp_IndexBuffer;
		uint32_t m_NumIndices = 0;
		uint32_t m_MaterialId = 0;
		std::string m_MaterialName = std::string();
	};

	class MSAPI ModelDx11 : public Model
	{
		friend class GraphicsDx11;
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mp_ConstBufferMVP;
		std::vector<MeshDx11> mv_Meshes;
	};

	class MSAPI Material
	{
		friend class GraphicsDx11;
	public: // Setters
		inline void SetBaseColor(const glm::vec4& color) noexcept { m_BaseColor = color; }
		inline void SetSubColor(const glm::vec4& color) noexcept { m_BaseColor = color; }
		inline void SetSpecularPower(const float& power) noexcept { m_SpecularPower = power; }
		inline void SetSpecularTextureId(const uint32_t& id) noexcept { m_SpecularTextureId = id; }
		inline void SetNormalTextureId(const uint32_t& id) noexcept { m_NormalTextureId = id; }
		inline void SetDiffuseTextureId(const uint32_t& id) noexcept { m_DiffuseTextureId = id; }

	public: // Getters
		inline glm::vec4 GetBaseColor() const noexcept { return m_BaseColor; }
		inline glm::vec4 GetSubColor() const noexcept { return m_SubColor; }
		inline float GetSpecularPower() const noexcept { return m_SpecularPower; }
		inline uint32_t GetSpecularTextureId() const noexcept { return m_SpecularTextureId; }
		inline uint32_t GetNormalTextureId() const noexcept { return m_NormalTextureId; }
		inline uint32_t GetDiffuseTextureId() const noexcept { return m_DiffuseTextureId; }
		inline uint32_t GetMaterialUID() const noexcept { return m_MaterialId; }
		inline std::string GetMaterialName() const noexcept { return m_MaterialName; }

	private:
		float m_SpecularPower = 1.0f;
		glm::vec4 m_BaseColor = glm::vec4(1.0f);
		glm::vec4 m_SubColor = glm::vec4(0.0f);
		uint32_t m_NormalTextureId = 0;
		uint32_t m_DiffuseTextureId = 0;
		uint32_t m_SpecularTextureId = 0;
		uint32_t m_MaterialId = 0;
		std::string m_MaterialName = std::string();
	};
}