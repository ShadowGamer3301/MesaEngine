#include "Core.h"

inline std::string PackData(const std::string& path, const std::string& targetPath)
{
	std::ifstream file(path);

	std::string currentPackName = std::string();
	uint32_t currentIndex = 0;
	std::vector<Entry> v_Entries;

	std::ostringstream oss;

	// Ensure that the PCDEF file is open
	if (file.is_open())
	{
		std::string line;
		// Read file line by line
		while (std::getline(file, line))
		{
			// $ sign marks beggining of new package
			if (line[0] == '$')
			{
				// Remove $ sing from package name
				currentPackName = line.substr(1, line.size() - 1);
				currentIndex = 0;
				// Skip to the next line
				continue;
			}

			// Generate has of a file
			std::stringstream hashStream;
			hashStream << std::hex << Mesa::FileUtils::HashFile(line) << std::dec;

			// Fill out entry data
			Entry entry = {};
			entry.m_Index = currentIndex;
			entry.m_OriginalName = line;
			entry.m_PackName = currentPackName;
			entry.m_Hash = hashStream.str();
			entry.m_OriginalSize = Mesa::FileUtils::FileSize(line);

			// Push entry data to vector
			v_Entries.push_back(entry);

			// Calculate index for next entry
			currentIndex++;
		}
	}

	// Split all entires into their respective archives
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

	// Calculate number of files in each archive
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

	// Write basic info to each archive
	for (const auto& value : numFilesMap)
	{
		uint8_t a_splitdata[4];
		memcpy(a_splitdata, &value.second, sizeof(uint32_t));

		std::vector<unsigned char> v_data = { a_splitdata[0], a_splitdata[1], a_splitdata[2], a_splitdata[3] };

		Mesa::FileUtils::MakeFileWithContent(value.first, v_data);
	}

	for (auto& archive : archivesMap)
	{
		// Write header for each archive
		LOG_F(INFO, "Writing header for %s ", archive.first.c_str());

		std::sort(archive.second.begin(), archive.second.end());

		uint64_t startPos = 0;
		uint32_t prevFileSize = 0;

		for (const auto& entry : archive.second)
		{
			if (entry.m_Index == 0)
			{
				// Calculate starting position (where the data begins) of the first file in archive
				// 4 bytes for number of files + (8 bytes for file starting pos + 4 bytes for file size) * number of files in archive + 1 byte (without it starting position would point to the last byte of the header)
				startPos = sizeof(uint32_t) + (sizeof(uint64_t) + sizeof(uint32_t)) * archive.second.size() + 1;
				uint8_t a_file0start[8];
				memcpy(a_file0start, &startPos, sizeof(uint64_t));

				// Save file since since it will be needed in later calculations
				uint8_t a_file0size[4];
				memcpy(a_file0size, &entry.m_OriginalSize, sizeof(uint32_t));
				prevFileSize = entry.m_OriginalSize;

				// Copy calculated data into a buffer
				std::vector<unsigned char> v_EntryData(sizeof(uint64_t) + sizeof(uint32_t));

				memcpy(&v_EntryData[0], &a_file0start[0], sizeof(uint64_t));
				memcpy(&v_EntryData[sizeof(uint64_t)], &a_file0size[0], sizeof(uint32_t));

				// Write buffer to archive
				Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_EntryData);
			}
			else
			{
				// Calculate starting position of the file
				// starting position of previous file + size of previous file
				startPos = startPos + prevFileSize;
				uint8_t a_file0start[8];
				memcpy(a_file0start, &startPos, sizeof(uint64_t));

				// Save current file size of later calculations
				uint8_t a_file0size[4];
				memcpy(a_file0size, &entry.m_OriginalSize, sizeof(uint32_t));
				prevFileSize = entry.m_OriginalSize;

				// Copy calculated data to buffer
				std::vector<unsigned char> v_EntryData(sizeof(uint64_t) + sizeof(uint32_t));

				memcpy(&v_EntryData[0], &a_file0start[0], sizeof(uint64_t));
				memcpy(&v_EntryData[sizeof(uint64_t)], &a_file0size[0], sizeof(uint32_t));

				// Write buffer to archive
				Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_EntryData);
			}
		}

		LOG_F(INFO, "Written header for %s ", archive.first.c_str());

		// Start writing file data to archive

		LOG_F(INFO, "Writing data for %s ", archive.first.c_str());

		for (const auto& entry : archive.second)
		{
			// Copy contents of the file into buffer
			auto v_fileData = Mesa::FileUtils::ReadBinaryData(entry.m_OriginalName);
			// Write buffer to archive
			Mesa::FileUtils::AppendDataToFile(entry.m_PackName, v_fileData);
		}
	}

	// Generate data for lookup table
	for (auto& entry : v_Entries)
	{
		oss << entry.m_OriginalName << "," << entry.m_PackName << "," << entry.m_Index << "," << entry.m_Hash << "," << entry.m_OriginalSize << "\n";
	}

	std::string result = oss.str();

	for (const auto& archive : archivesMap)
	{
		std::string newFileName = Mesa::FileUtils::CombinePaths(targetPath, archive.first);

		BOOL mr = MoveFile(Mesa::ConvertUtils::StringToWideString(archive.first).c_str(), Mesa::ConvertUtils::StringToWideString(newFileName).c_str());
		if (mr == 0)
		{
			LOG_F(ERROR, "Failed to move archive to its destignated path!");
			throw Mesa::Exception();
		}
		
	}

	// Return data for lookup table
	return result;
}

inline void CreateTree(const std::vector<std::string>& v_Directories)
{
	std::string prevDirs = std::string();

	for (auto& entry : v_Directories)
	{
		std::string finalDir = Mesa::FileUtils::CombinePaths(prevDirs, entry);

		BOOL result = CreateDirectory(Mesa::ConvertUtils::StringToWideString(finalDir).c_str(), nullptr);

		if (result == 0)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				LOG_F(ERROR, "Failed to create directory: %s", entry.c_str());
				throw Mesa::Exception();
			}
		}

		prevDirs = finalDir;
	}
}

inline void ValidateDirectories()
{
	std::string shaderDir = Mesa::ConfigUtils::GetValueFromConfigCS("PATH", "Shader");
	std::string modelDir = Mesa::ConfigUtils::GetValueFromConfigCS("PATH", "Model");
	std::string textureDir = Mesa::ConfigUtils::GetValueFromConfigCS("PATH", "Texture");
	std::string materialDir = Mesa::ConfigUtils::GetValueFromConfigCS("PATH", "Material");

	std::vector<std::string> v_ShaderPath = Mesa::ConvertUtils::SplitStringByChar(shaderDir, '/');

	if (v_ShaderPath.size() <= 1)
		v_ShaderPath = Mesa::ConvertUtils::SplitStringByChar(shaderDir, '\\');

	CreateTree(v_ShaderPath);

	std::vector<std::string> v_TexturePath = Mesa::ConvertUtils::SplitStringByChar(textureDir, '/');
	if (v_TexturePath.size() <= 1)
		v_TexturePath = Mesa::ConvertUtils::SplitStringByChar(textureDir, '\\');

	CreateTree(v_TexturePath);

	std::vector<std::string> v_ModelPath = Mesa::ConvertUtils::SplitStringByChar(modelDir, '/');
	if (v_ModelPath.size() <= 1)
		v_ModelPath = Mesa::ConvertUtils::SplitStringByChar(modelDir, '\\');

	CreateTree(v_ModelPath);

	std::vector<std::string> v_MaterialPath = Mesa::ConvertUtils::SplitStringByChar(materialDir, '/');
	if (v_MaterialPath.size() <= 1)
		v_MaterialPath = Mesa::ConvertUtils::SplitStringByChar(materialDir, '\\');

	CreateTree(v_MaterialPath);
}

int main(void) try
{
	if (!Mesa::FileUtils::FileExists("engine.ini"))
		Mesa::ConfigUtils::GenerateConfig();

	ValidateDirectories();

	std::string lookupData = std::string();

	// Look for the file containing info on how to pack textures
	if (Mesa::FileUtils::FileExists("textures.pcdef"))
	{
		LOG_F(INFO, "Packing textures...");
		// Append generated lookup data to already existing data 
		lookupData += PackData("textures.pcdef", Mesa::ConfigUtils::GetValueFromConfigCS("Path", "Texture"));
	}

	// Look for the file containing info on how to pack materials
	if (Mesa::FileUtils::FileExists("materials.pcdef"))
	{
		LOG_F(INFO, "Packing materials...");
		// Append generated lookup data to already existing data
		lookupData += PackData("materials.pcdef", Mesa::ConfigUtils::GetValueFromConfigCS("Path", "Material"));
	}

	// Look for the file containing info on how to pack directx shaders
	if (Mesa::FileUtils::FileExists("shaders_dx.pcdef"))
	{
		LOG_F(INFO, "Packing DirectX shaders...");
		// Append generated lookup data to already existing data
		lookupData += PackData("shaders_dx.pcdef", Mesa::ConfigUtils::GetValueFromConfigCS("Path", "Shader"));
	}

	// Look for the file containing info on how to pack models
	if (Mesa::FileUtils::FileExists("models.pcdef"))
	{
		LOG_F(INFO, "Packing models...");
		// Append generated lookup data to already existing data
		lookupData += PackData("models.pcdef", Mesa::ConfigUtils::GetValueFromConfigCS("Path", "Model"));
	}

	// Generate lookup table that will be used for loading assets
	LOG_F(INFO, "Generating lookup table...");
	Mesa::FileUtils::MakeFileWithContent("lookup.csv", lookupData);
	LOG_F(INFO, "Lookup table generated");

	return 0;
}
catch (Mesa::Exception& me)
{
	LOG_F(ERROR, "%s", me.what());
	return 1;
}