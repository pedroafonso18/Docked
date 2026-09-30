#include "config.h"

#include <filesystem>
#include "helpers.h"

Config::Config(
    const std::string& projectPath
) {
    values = Helpers::ParseConfigFile(projectPath);
}

ConfigValues Config::GetValues() {
    return values;
}

ConfigValues Config::LoadConfigForPath(
    const std::string& projectPath
)
{
    const auto configPath = std::filesystem::path(projectPath) / CONFIG_FILE_NAME;

    if (!std::filesystem::exists(configPath)) {
        return ConfigValues{};
    }

    return Helpers::ParseConfigFile(configPath.string());
}