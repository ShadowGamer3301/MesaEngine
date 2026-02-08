#include <Mesa/CompressionUtils.h>
#include <Mesa/FileUtils.h>

namespace Mesa
{
	std::vector<unsigned char> CompressionUtils::DecompressFile(const uint32_t& srcLen, const std::string& inputPath)
	{
		if (!FileUtils::FileExists(inputPath))
		{
			LOG_F(ERROR, "%s does not exists in provided path!", inputPath.c_str());
			return std::vector<unsigned char>();
		}

		std::vector<unsigned char> v_data = FileUtils::ReadBinaryData(inputPath);
		std::vector<unsigned char> result;
		size_t dataSize = v_data.size();
		result.resize(srcLen);

		int decompResult = lzav_decompress(v_data.data(), result.data(), v_data.size(), srcLen);
		if (decompResult < 0)
		{
			LOG_F(ERROR, "Decompression of %s failed!", inputPath.c_str());
			return std::vector<unsigned char>();
		}

		return result;
	}

	std::vector<unsigned char> CompressionUtils::CompressFile(const std::string& inputPath)
	{
		if (!FileUtils::FileExists(inputPath))
		{
			LOG_F(ERROR, "%s does not exists in provided path!", inputPath.c_str());
			return std::vector<unsigned char>();
		}

		std::vector<unsigned char> v_data = FileUtils::ReadBinaryData(inputPath);

		size_t dataSize = v_data.size();
		int maxLen = lzav_compress_bound(dataSize);
		std::vector<unsigned char> outBuffer(maxLen);

		int compLen = lzav_compress_default(v_data.data(), outBuffer.data(), dataSize, maxLen);

		if (compLen == 0 && dataSize != 0)
		{
			LOG_F(ERROR, "Compression of %s failed!", inputPath.c_str());
			return std::vector<unsigned char>();
		}

		return outBuffer;
	}

	std::vector<unsigned char> CompressionUtils::DecompressData(const uint32_t& srcLen, const std::vector<unsigned char>& data)
	{
		std::vector<unsigned char> result;

		size_t dataSize = data.size();
		result.resize(srcLen);
		
		int decompResult = lzav_decompress(data.data(), &result[0], data.size(), srcLen);
		if (decompResult < 0 && decompResult != -3)
		{
			LOG_F(ERROR, "Decompression failed!");
			return std::vector<unsigned char>();
		}

		return result;
	}

}

