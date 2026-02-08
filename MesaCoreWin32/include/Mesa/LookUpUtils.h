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
		std::vector<LookUpEntry> LoadLookupTable();
	};
}