#include <Mesa/ConfigUtils.h>
#include <Mesa/Exception.h>
#include <Mesa/ConvertUtils.h>

namespace Mesa
{
    /*
       Retrieves a value from the default configuration file ("engine.ini").
    */
    std::string ConfigUtils::GetValueFromConfig(const std::string& section, const std::string& key)
    {
        std::string result = std::string();

        // Initialize the handler for the default engine configuration file.
        mINI::INIFile iniFile("engine.ini");
        mINI::INIStructure iniStruct;

        // Attempt to parse the file into the internal data structure.
        if (iniFile.read(iniStruct))
            result = iniStruct[section][key];
        else
            result = "";

        // Convert to lowercase to ensure that comparisons (like "true" vs "True") are case-insensitive throughout the engine.
        return ConvertUtils::ToLowerCase(result);
    }

    /*
        Retrieves a value from a specified custom configuration file.
    */
    std::string ConfigUtils::GetValueFromCustomConfig(const std::string& file, const std::string& section, const std::string& key)
    {
        std::string result = std::string();

        mINI::INIFile iniFile(file);
        mINI::INIStructure iniStruct;

        // Load and access the specific file provided in the arguments.
        if (iniFile.read(iniStruct))
            result = iniStruct[section][key];
        else
            result = "";

        return ConvertUtils::ToLowerCase(result);
    }

    /*
        Retrieves a value from "engine.ini" while preserving its original casing.
    */
    std::string ConfigUtils::GetValueFromConfigCS(const std::string& section, const std::string& key)
    {
        std::string result = "";

        mINI::INIFile iniFile("engine.ini");
        mINI::INIStructure iniStruct;

        // Read the file and return the exact string found in the INI.
        if (iniFile.read(iniStruct))
        {
            result = iniStruct[section][key];
        }

        return result;
    }

    /*
        Retrieves a value from a custom configuration file while preserving its original casing.
    */
    std::string ConfigUtils::GetValueFromCustomConfigCS(const std::string& file, const std::string& section, const std::string& key)
    {
        std::string result = "";

        mINI::INIFile iniFile(file);
        mINI::INIStructure iniStruct;

        if (iniFile.read(iniStruct))
        {
            result = iniStruct[section][key];
        }

        return result;
    }

    /*
        Generates a default "engine.ini" file with factory settings.
        This function is called if the config file is missing on startup.
    */
    void ConfigUtils::GenerateConfig()
    {
        mINI::INIStructure iniStruct;

        // --- Debug & Logging Settings ---
        iniStruct["Debug"]["Log"] = "False";
        iniStruct["Debug"]["LogPath"] = "";
        iniStruct["Debug"]["LogType"] = "Truncate";
        iniStruct["Debug"]["LogName"] = "mesa.log.txt";

        // --- General Engine Settings ---
        iniStruct["General"]["Api"] = "dx11";

        // --- Window Settings ---
        iniStruct["Window"]["Width"] = "800";
        iniStruct["Window"]["Height"] = "600";
        iniStruct["Window"]["Fullscreen"] = "False";

        // --- DX11 Specific Graphics Settings ---
        iniStruct["Graphics_Dx11"]["RenderDistance"] = "1000.0f";
        iniStruct["Graphics_Dx11"]["SamplingMode"] = "Linear";
        iniStruct["Graphics_Dx11"]["AntiAliasing"] = "Fxaa";

        // --- Resource Paths ---
        // These define where the engine looks for assets.
        iniStruct["Path"]["Shader"] = "Asset/Shader/";
        iniStruct["Path"]["Model"] = "Asset/Model/";
        iniStruct["Path"]["Texture"] = "Asset/Texture/";
        iniStruct["Path"]["Material"] = "Asset/Material/";

        // Finalize the file creation.
        mINI::INIFile iniFile("engine.ini");

        // If the library fails to write to the disk throw exception.
        if (!iniFile.generate(iniStruct))
            throw Exception();
    }
}