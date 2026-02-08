#pragma once
#include "Core.h"

namespace Mesa
{
	class MSAPI CompressionUtils
	{
	public:
		static std::vector<unsigned char> DecompressFile(const uint32_t& srcLen, const std::string& inputPath);
		static std::vector<unsigned char> CompressFile(const std::string& inputPath);
		static std::vector<unsigned char> DecompressData(const uint32_t& srcLen, const std::vector<unsigned char>& data);
	};
}