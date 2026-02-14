#pragma once
#include <Mesa/ConvertUtils.h>

namespace Mesa
{
	/*
		Converts std::string to std::wstring.
	*/
	std::wstring ConvertUtils::StringToWideString(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}

	/*
		Splits string by specified character into vector of substrings.
	*/
	std::vector<std::string> ConvertUtils::SplitStringByChar(const std::string& s, char c)
	{
		// Wrap the string in a stream to use stream-based parsing.
		std::istringstream iss(s);
		std::string split;
		std::vector<std::string> v_result;

		// Continuously extract characters until the next delimiter 'c' is found.
		while (std::getline(iss, split, c))
			v_result.push_back(split);

		return v_result;
	}

	/*
		Converts string to float. 
		If the string cannot be converted to float returns 0.0f.
	*/
	float ConvertUtils::StringToFloat(const std::string& s)
	{
		try
		{
			// Attempt to parse the string into a float.
			return std::stof(s);
		}
		catch (const std::exception&)
		{
			// If parsing fails (e.g., non-numeric string), return a default value.
			return 0.0f;
		}
	}

	/*
		Turns every uppercase character in string into its lowercase equivalent.
	*/
	std::string ConvertUtils::ToLowerCase(const std::string& s)
	{
		// Walk through the string and apply tolower() to each character.
		// Cast to unsigned char to avoid undefined behavior with certain locales.
		std::string result = s;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
		return result;
	}

	/*
		Converts string to int.
		If the string cannot be converted to int returns 0.
	*/
	int ConvertUtils::StringToInt(const std::string& s)
	{
		try
		{
			// Attempt to parse the string into a int.
			return std::stoi(s);
		}
		catch (const std::exception&)
		{
			// If parsing fails (e.g., non-numeric string), return a default value.
			return 0;
		}
	}

	/*
		Converts std::array of 4 floats into XMFLOAT4 structure.
	*/
	DirectX::XMFLOAT4 ConvertUtils::ArrayToXmFloat4(const std::array<float, 4>& data)
	{
		return DirectX::XMFLOAT4(data[0], data[1], data[2], data[3]);
	}

	/*
		Converts glm::mat4x4 to XMMATRIX structure ensuring
		that data is still ordered properly.
	*/
	DirectX::XMMATRIX ConvertUtils::Mat4x4ToXmMatrix(const glm::mat4x4& m)
	{
		// Create temporary buffer to hold the data
		DirectX::XMFLOAT4X4 temp;

		// Copy data from glm matrix into buffer
		// converting order from column major (used by GLM) 
		// to row major (used by DirectXMath)
		temp.m[0][0] = m[0][0];
		temp.m[0][1] = m[1][0];
		temp.m[0][2] = m[2][0];
		temp.m[0][3] = m[3][0];

		temp.m[1][0] = m[0][1];
		temp.m[1][1] = m[1][1];
		temp.m[1][2] = m[2][1];
		temp.m[1][3] = m[3][1];

		temp.m[2][0] = m[0][2];
		temp.m[2][1] = m[1][2];
		temp.m[2][2] = m[2][2];
		temp.m[2][3] = m[3][2];

		temp.m[3][0] = m[0][3];
		temp.m[3][1] = m[1][3];
		temp.m[3][2] = m[2][3];
		temp.m[3][3] = m[3][3];

		// Copy the results from buffer to XMMATIRX
		DirectX::XMMATRIX result = DirectX::XMLoadFloat4x4(&temp);

		return result;
	}

	/*
		Converts glm::vec3 to XMFLOAT3 structure.
	*/
	DirectX::XMFLOAT3 ConvertUtils::Vec3ToXmFloat3(const glm::vec3& data)
	{
		return DirectX::XMFLOAT3(data.x, data.y, data.z);
	}

	std::string ConvertUtils::RemoveCharFromString(const std::string& s, char c)
	{
		std::string str = s;
		str.erase(std::remove(str.begin(), str.end(), c), str.end());
		return str;
	}

	std::string ConvertUtils::ReplaceCharInString(const std::string& s, char original, char replacement)
	{
		std::string result = s;
		std::replace(result.begin(), result.end(), original, replacement);
		return result;
	}
}