#include "Core.h"

inline std::string PackData(const std::string& path)
{
	std::ifstream file(path);

	std::string currentPackName = std::string();
	uint32_t currentIndex = 0;
	std::vector<Entry> v_Entries;

	std::ostringstream oss;

	if (file.is_open())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line[0] == '$')
			{
				currentPackName = line.substr(1, line.size() - 1);
				currentIndex = 0;
				continue;
			}

			std::stringstream hashStream;
			hashStream << std::hex << Mesa::FileUtils::HashFile(line) << std::dec;

			Entry entry = {};
			entry.m_Index = currentIndex;
			entry.m_OriginalName = line;
			entry.m_PackName = currentPackName;
			entry.m_Hash = hashStream.str();
			entry.m_OriginalSize = Mesa::FileUtils::FileSize(line);

			v_Entries.push_back(entry);
			currentIndex++;
		}
	}

	std::map<std::string, std::vector<Entry>> archivesMap;

	for (const auto& entry : v_Entries)
	{
		if (archivesMap.find(entry.m_PackName) == archivesMap.end())
		{
			archivesMap[entry.m_PackName] = std::vector<Entry>();
			archivesMap[entry.m_PackName].push_back(entry);
		}
		else
		{
			archivesMap[entry.m_PackName].push_back(entry);
		}
	}

	std::map<std::string, uint32_t> numFilesMap;

	for (const auto& entry : v_Entries)
	{
		if (numFilesMap.find(entry.m_PackName) == numFilesMap.end())
		{
			numFilesMap[entry.m_PackName] = 1;
		}
		else
		{
			numFilesMap[entry.m_PackName]++;
		}
	}

	for (const auto& value : numFilesMap)
	{
		uint8_t a_splitdata[4];
		memcpy(a_splitdata, &value.second, sizeof(uint32_t));

		std::vector<unsigned char> v_data = { a_splitdata[0], a_splitdata[1], a_splitdata[2], a_splitdata[3] };

		Mesa::FileUtils::MakeFileWithContent(value.first, v_data);
	}

	for (auto& archive : archivesMap)
	{
		LOG_F(INFO, "Writing header for %s ", archive.first.c_str());

		std::sort(archive.second.begin(), archive.second.end());

		uint64_t startPos = 0;
		uint32_t prevFileSize = 0;

		for (const auto& entry : archive.second)
		{
			if (entry.m_Index == 0)
			{
				startPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * archive.second.size() + 1;
				uint8_t a_file0start[8];
				memcpy(a_file0start, &startPos, sizeof(uint64_t));

				uint8_t a_file0size[4];
				memcpy(a_file0size, &entry.m_OriginalSize, sizeof(uint32_t));
				prevFileSize = entry.m_OriginalSize;

				std::vector<unsigned char> v_EntryData(sizeof(uint64_t) + sizeof(uint32_t));

				memcpy(&v_EntryData[0], &a_file0start[0], sizeof(uint64_t));
				memcpy(&v_EntryData[sizeof(uint64_t)], &a_file0size[0], sizeof(uint32_t));

				Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_EntryData);
			}
			else
			{
				startPos = startPos + prevFileSize;
				uint8_t a_file0start[8];
				memcpy(a_file0start, &startPos, sizeof(uint64_t));

				uint8_t a_file0size[4];
				memcpy(a_file0size, &entry.m_OriginalSize, sizeof(uint32_t));
				prevFileSize = entry.m_OriginalSize;

				std::vector<unsigned char> v_EntryData(sizeof(uint64_t) + sizeof(uint32_t));

				memcpy(&v_EntryData[0], &a_file0start[0], sizeof(uint64_t));
				memcpy(&v_EntryData[sizeof(uint64_t)], &a_file0size[0], sizeof(uint32_t));
				Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_EntryData);
			}
		}

		LOG_F(INFO, "Written header for %s ", archive.first.c_str());
		LOG_F(INFO, "Writing data for %s ", archive.first.c_str());

		for (const auto& entry : archive.second)
		{
			auto v_fileData = Mesa::FileUtils::ReadBinaryData(entry.m_OriginalName);
			Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_fileData);
		}
	}

	for (auto& entry : v_Entries)
	{
		oss << entry.m_OriginalName << "," << entry.m_PackName << "," << entry.m_Index << "," << entry.m_Hash << "," << entry.m_OriginalSize << "\n";
	}

	std::string result = oss.str();

	return result;
}

int main(int argc, char** argv)
{
	std::string lookupData = std::string();

	if (!Mesa::FileUtils::FileExists("lookup.csv"))
	{
		LOG_F(INFO, "Generating lookup file...");
		Mesa::FileUtils::MakeFile("lookup.csv");
	}

	if (Mesa::FileUtils::FileExists("textures.pcdef"))
	{
		LOG_F(INFO, "Packing textures...");
		lookupData += PackData("textures.pcdef");
	}

	if (Mesa::FileUtils::FileExists("materials.pcdef"))
	{
		LOG_F(INFO, "Packing materials...");
		lookupData += PackData("materials.pcdef");
	}

	LOG_F(INFO, "Generating lookup table...");
	Mesa::FileUtils::MakeFileWithContent("lookup.csv", lookupData);
	LOG_F(INFO, "Lookup table generated");

	return 0;
}