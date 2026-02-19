#pragma once
#include "Core.h"

namespace Mesa
{
	class MSAPI FileUtils
	{
	public:
		static bool FileExists(std::string path);
		static std::string CombinePaths(const std::string& path1, const std::string& path2);
		static size_t FileSize(const std::string& path);
		static std::optional<size_t> FileSizeSafe(const std::string& path);
		static void MakeFile(const std::string& path);
		static void MakeFileWithContent(const std::string& path, const std::vector<unsigned char>& data);
		static void MakeFileWithContent(const std::string& path, const std::string& data);
		static uint32_t HashFile(const std::string& path);
		static uint32_t HashData(const std::vector<unsigned char>& data);
		static std::vector<unsigned char> ReadBinaryData(const std::string& path);
		static void AppendDataToFile(const std::string& path, const std::vector<unsigned char>& data);
		static std::string ReadTextData(const std::string& path);
		static std::string StripExtensionFromPath(const std::string& path);
		static std::string StripPathToFileName(const std::string& path);
		static std::vector<std::string> GetFileNamesInDirectory(const std::string& path);
		static std::vector<unsigned char> LoadDataChunk(const std::string& path, const size_t& size, const size_t& pos);
		static void AppendTextToFile(const std::string& path, const std::string& data);
	};
}