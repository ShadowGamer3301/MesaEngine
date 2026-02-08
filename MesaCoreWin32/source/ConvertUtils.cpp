#pragma once
#include <Mesa/ConvertUtils.h>

namespace Mesa
{
	std::wstring ConvertUtils::StringToWideString(const std::string& s)
	{
		return std::wstring();
	}
	std::vector<std::string> ConvertUtils::SplitStringByChar(const std::string& s, char c)
	{
		return std::vector<std::string>();
	}
	float ConvertUtils::StringToFloat(const std::string& s)
	{
		return 0.0f;
	}
	std::string ConvertUtils::ToLowerCase(const std::string& s)
	{
		return std::string();
	}
	int ConvertUtils::StringToInt(const std::string& s)
	{
		return 0;
	}
	DirectX::XMFLOAT4 ConvertUtils::ArrayToXmFloat4(const std::array<float, 4>& data)
	{
		return DirectX::XMFLOAT4();
	}
	DirectX::XMMATRIX ConvertUtils::Mat4x4ToXmMatrix(const glm::mat4x4& m)
	{
		return DirectX::XMMATRIX();
	}
	DirectX::XMFLOAT3 ConvertUtils::Vec3ToXmFloat3(const glm::vec3& data)
	{
		return DirectX::XMFLOAT3();
	}
}