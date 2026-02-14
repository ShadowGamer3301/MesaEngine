#pragma once
#include "Core.h"

namespace Mesa
{
	namespace ConstBufferDx11
	{
		struct alignas(16) MvpBuffer
		{
			DirectX::XMMATRIX m_Model;
			DirectX::XMMATRIX m_View;
			DirectX::XMMATRIX m_Proj;
		};

		struct alignas(16) MaterialBufferColorPass
		{
			DirectX::XMFLOAT4 m_BaseColor;
			DirectX::XMFLOAT4 m_SubColor;
		};

		struct alignas(16) MaterialBufferSpecularPass
		{
			float m_SpecularPower;
		};
	}
}