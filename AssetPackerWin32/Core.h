#pragma once
#include <Mesa/Core.h>
#include <Mesa/FileUtils.h>
#include <Mesa/CompressionUtils.h>
#include <Mesa/ConfigUtils.h>
#include <Mesa/ConvertUtils.h>
#include <Mesa/Exception.h>

struct Entry
{
	std::string m_OriginalName;
	std::string m_PackName;
	std::string m_Hash;
	uint32_t m_Index;
	uint32_t m_OriginalSize;

	inline bool operator<(const Entry& e) const
	{
		return (m_Index < e.m_Index);
	}
	inline bool operator>(const Entry& e) const
	{
		return (m_Index > e.m_Index);
	}
};

struct Archive
{
	std::string m_ArchiveName;
	std::vector<Entry> mv_Entries;
};