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
	}
}