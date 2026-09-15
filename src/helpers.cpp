#include "helpers.h"

#include <cctype>
#include <fstream>
#include "../vendor/toml.h"

namespace {

std::string ToUpper(
    std::string value
)
{
    for (char& character : value) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }

    return value;
}

ConfigVariables::ProjectLanguage ParseProjectLanguageInfo(
    const std::string& languageString
)
{
    const std::string normalizedLanguage = ToUpper(languageString);

    if (normalizedLanguage == "C") {
        return ConfigVariables::C;
    }

    return ConfigVariables::CPP;
}

ConfigVariables::Compiler ParseCompilerInfo(
    const std::string& compilerString
)
{
    const std::string normalizedCompiler = ToUpper(compilerString);

    if (normalizedCompiler == "CLANG") {
        return ConfigVariables::CLANG;
    } else if (normalizedCompiler == "GCC" || normalizedCompiler == "G++") {
        return ConfigVariables::GCC;
    } else if (normalizedCompiler == "MSVC") {
        return ConfigVariables::MSVC;
    }

    return ConfigVariables::DEFAULT;
}

ConfigVariables::Standard ParseStandardInfo(
    int standardValue
)
{
    switch (standardValue) {
        case 14:
            return ConfigVariables::FOURTEEN;
        case 17:
            return ConfigVariables::SEVENTEEN;
        case 20:
            return ConfigVariables::TWENTY;
        case 23:
            return ConfigVariables::TWENTY_THREE;
        case 26:
        default:
            return ConfigVariables::TWENTY_SIX;
    }
}

std::vector<std::string> ParseDependenciesInfo(
    const toml::table* dependenciesTable
)
{
    std::vector<std::string> dependencies;

    if (dependenciesTable == nullptr) {
        return dependencies;
    }

    for (const auto& [dependencyName, dependencyVersion] : *dependenciesTable) {
        dependencies.push_back(
            std::string(dependencyName.str()) + "=" + dependencyVersion.value_or("")
        );
    }

    return dependencies;
}

} // namespace

std::string Helpers::Trim(
    const std::string& value
)
{
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

ConfigValues Helpers::ParseConfigFile(
    const std::string& path
)
{
    ConfigValues values{};

    std::ifstream configFile(path);
    if (!configFile.is_open()) {
        return values;
    }

    try {
        const auto config = toml::parse_file(path);

        values.ProjectName = config["project"]["name"].value_or("");
        values.ProjectVersion = config["project"]["version"].value_or(0.0);
        values.ProjectLanguage = ParseProjectLanguageInfo(config["project"]["language"].value_or("C++"));
        values.Compiler = ParseCompilerInfo(config["project"]["compiler"].value_or("default"));
        values.Standard = ParseStandardInfo(config["project"]["standard"].value_or(26));
        values.Dependencies = ParseDependenciesInfo(config["dependencies"].as_table());
    } catch (const std::exception&) {
        return ConfigValues{};
    }

    return values;
}