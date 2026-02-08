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

	class MSAPI Shader
	{
	public:
		inline uint32_t GetShaderUID() const noexcept { return m_ShaderUID; }
		inline ShaderType GetShaderType() const noexcept { return m_ShaderType; }

	protected:
		uint32_t m_ShaderUID = 0;
		ShaderType m_ShaderType = ShaderType_Forward;
	};

	class MSAPI ShaderDx11 : public Shader
	{
		friend class GraphicsDx11;
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mp_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mp_InputLayout;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mp_PixelShader;
	};
	
}