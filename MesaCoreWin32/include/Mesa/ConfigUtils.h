#pragma once
#include "Core.h"

namespace Mesa
{
	class MSAPI ConfigUtils
	{
	public:
		static std::string GetValueFromConfig(const std::string& section, const std::string& key);
		static std::string GetValueFromCustomConfig(const std::string& file, const std::string& section, const std::string& key);
		static std::string GetValueFromConfigCS(const std::string& section, const std::string& key);
		static std::string GetValueFromCustomConfigCS(const std::string& file, const std::string& section, const std::string& key);
		static void GenerateConfig();
	};
}