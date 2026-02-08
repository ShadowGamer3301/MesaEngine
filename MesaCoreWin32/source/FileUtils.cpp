#include <Mesa/FileUtils.h>

namespace Mesa
{
	/*
		Checks if a file or directory exists at the specified path.
	*/
	bool FileUtils::FileExists(std::string path)
	{
		return std::filesystem::exists(path);
	}

	/*
		Combines two path strings into one, ensuring a proper directory separator exists between them.
	*/
	std::string FileUtils::CombinePaths(const std::string& path1, const std::string& path2)
	{
		// If the base path is empty, returning path2 prevents a leading slash error.
		if (path1.empty()) return path2;

		size_t len = path1.length();
		std::string result = std::string();

		// Check if the last character of path1 is already a directory separator.
		if (path1[len - 1] == '/' || path1[len - 1] == '\\')
		{
			// If it has a slash, simply concatenate the two strings.
			result = path1 + path2;
		}
		else
		{
			// If it lacks a slash, manually insert a '/' before appending path2.
			result = path1 + '/' + path2;
		}

		return result;
	}

	/*
		 Calculates the size of a specific file.
		 If the file cannot be measured returns 0.
	*/
	size_t FileUtils::FileSize(const std::string& path)
	{
		try
		{
			// std::filesystem::file_size returns the size in bytes.
			size_t result = std::filesystem::file_size(path);
			return result;
		}
		catch (const std::filesystem::filesystem_error& fse)
		{
			// If the file is missing, access is denied, or the path is invalid,
			// we catch the exception and log the specific error message.
			LOG_F(ERROR, "%s", fse.what());

			// Return 0 as a safe fallback, indicating the file couldn't be measured.
			return 0;
		}
	}

	/*
		 Calculates the size of a specific file.
		 If the file cannot be measured returns optional with no value.
	*/
	std::optional<size_t> FileUtils::FileSizeSafe(const std::string& path)
	{
		try
		{
			// std::filesystem::file_size returns the size in bytes.
			size_t result = std::filesystem::file_size(path);
			return result;
		}
		catch (const std::filesystem::filesystem_error& fse)
		{
			// If the file is missing, access is denied, or the path is invalid,
			// we catch the exception and log the specific error message.
			LOG_F(ERROR, "%s", fse.what());

			// Return empty optional, indicating the file couldn't be measured.
			return std::optional<size_t>();
		}
	}

	/*
		Creates a file in specified directory only when it doesnt exists.
	*/
	void FileUtils::MakeFile(const std::string& path)
	{
		// Validate that the file doesn't already exist
		if (FileExists(path)) return;

		// Create the file
		std::ofstream output(path);

		// If the file is opened close it
		if (output.is_open()) output.close();
	}

	/*
		Creates a file in specified directory only when it doesnt exists.
		If the file exists this function truncates it and replaces its
		contents with the one provided in argument.
	*/
	void FileUtils::MakeFileWithContent(const std::string& path, const std::vector<unsigned char>& data)
	{
		// Open the file in truncate mode (if it doesn't exist it will be automatically created)
		std::ofstream output(path, std::ios::binary | std::ios::trunc);
		
		// Write data as binary data to the file
		output.write((const char*)data.data(), data.size());
		output.flush();

		// Close the file
		output.close();
	}

	void FileUtils::MakeFileWithContent(const std::string& path, const std::string& data)
	{
		std::ofstream output(path, std::ios::trunc);
		output << data;
		output.flush();
		output.close();
	}

	uint32_t FileUtils::HashFile(const std::string& path)
	{
		if (!FileExists(path)) return 0;

		std::ifstream file(path, std::ios::binary);

		std::vector<unsigned char> v_buffer(std::istreambuf_iterator<char>(file), {});
		
		file.close();

		uint32_t result = crc32c::Crc32c(v_buffer.data(), v_buffer.size());

		return result;
	}

	uint32_t FileUtils::HashData(const std::vector<unsigned char>& v_data)
	{
		uint32_t result = crc32c::Crc32c(v_data.data(), v_data.size());

		return result;
	}
	std::vector<unsigned char> FileUtils::ReadBinaryData(const std::string& path)
	{
		if (!FileExists(path)) return std::vector<unsigned char>();

		std::ifstream file(path, std::ios::binary);
		std::vector<unsigned char> v_buffer(std::istreambuf_iterator<char>(file), {});
		file.close();

		return v_buffer;
	}

	void FileUtils::AppendDataToFile(const std::string& path, const std::vector<unsigned char>& data)
	{
		// Open the file in appended mode (we can only write at the end of the file)
		// so we cannot override original data
		std::ofstream output(path, std::ios::binary | std::ios::app);

		// Write data as binary data to the file
		output.write((const char*)data.data(), data.size());
		output.flush();

		// Close the file
		output.close();
	}
}