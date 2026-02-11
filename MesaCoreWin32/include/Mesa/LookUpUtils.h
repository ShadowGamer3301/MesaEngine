#pragma once
#include "Core.h"

namespace Mesa
{
	struct LookUpEntry
	{
		std::string m_OriginalName;
		std::string m_PackName;
		uint32_t m_Index;
		uint32_t m_Hash;
		uint32_t m_Size;
	};

	class MSAPI LookUpUtils
	{
	public:
		static std::vector<LookUpEntry> LoadLookupTable();
		static std::vector<LookUpEntry> LoadSpecificPackInfo(const std::string& packName);
		static std::string FindFilePack(const std::string& fileName);
		static std::optional<uint32_t> FindFileIndex(const std::string& fileName);
		static std::vector<std::string> GetFileNamesFromPack(const std::string& packName);
		static std::string GetFileNameFromPack(const std::string& packName, const uint32_t index);
	};
}