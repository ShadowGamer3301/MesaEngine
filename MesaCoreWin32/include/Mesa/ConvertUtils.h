#pragma once
#include "Core.h"

namespace Mesa
{
	class MSAPI ConvertUtils
	{
	public:
		static std::wstring StringToWideString(const std::string& s);
		static std::vector<std::string> SplitStringByChar(const std::string& s, char c);
		static float StringToFloat(const std::string& s);
		static std::string ToLowerCase(const std::string& s);
		static int StringToInt(const std::string& s);
		static DirectX::XMFLOAT4 ArrayToXmFloat4(const std::array<float, 4>& data);
		static DirectX::XMMATRIX Mat4x4ToXmMatrix(const glm::mat4x4& m);
		static DirectX::XMFLOAT3 Vec3ToXmFloat3(const glm::vec3& data);
		static std::string RemoveCharFromString(const std::string& s, char c);
		static std::string ReplaceCharInString(const std::string& s, char original, char replacement);
		static DirectX::XMFLOAT4 Vec4ToXmFloat4(const glm::vec4& data);
	};
}