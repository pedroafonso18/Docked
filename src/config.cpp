#include "config.h"

#include "helpers.h"

Config::Config(
    const std::string& projectPath
) {
    values = Helpers::ParseConfigFile(projectPath);
}

ConfigValues Config::GetValues() {
    return values;
}