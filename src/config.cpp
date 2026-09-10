#include "config.h"

#include <iostream>
#include <fstream>
#include "../vendor/toml.h"

Config::Config(
    const std::string& projectPath
) {
    auto config = toml::parse_file(projectPath);
}
