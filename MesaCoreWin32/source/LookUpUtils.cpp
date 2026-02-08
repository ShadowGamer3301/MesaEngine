#include <Mesa/LookUpUtils.h>
#include <Mesa/FileUtils.h>
#include <Mesa/ConvertUtils.h>

namespace Mesa
{
	std::vector<LookUpEntry> LookUpUtils::LoadLookupTable()
	{
		std::string fileData = FileUtils::ReadTextData("lookup.csv");

		std::vector<std::string> v_lines = ConvertUtils::SplitStringByChar(fileData, '\n');

		std::vector<LookUpEntry> v_result;

		for (const auto& line : v_lines)
		{
			if (strcmp(line.c_str(), "\n") == 0) continue;

			std::vector<std::string> v_details = ConvertUtils::SplitStringByChar(fileData, ',');
			
			if (v_details.size() < 5)
			{
				LOG_F(ERROR, "Invalid entry detected in lookup table! Skipping...");
				continue;
			}

			LookUpEntry entry = {};
			entry.m_OriginalName = v_details[0];
			entry.m_PackName = v_details[1];
			entry.m_Index = ConvertUtils::StringToInt(v_details[2]);
			entry.m_Hash = ConvertUtils::StringToInt(v_details[3]);
			entry.m_Size = ConvertUtils::StringToInt(v_details[4]);

			v_result.push_back(entry);
		}

		return v_result;
	}
}
