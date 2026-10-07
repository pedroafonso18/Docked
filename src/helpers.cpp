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

ProjectLanguage ParseProjectLanguageInfo(
    const std::string& languageString
)
{
    const std::string normalizedLanguage = ToUpper(languageString);

    if (normalizedLanguage == "C") {
        return C;
    }

    return CPP;
}

Compiler ParseCompilerInfo(
    const std::string& compilerString
)
{
    const std::string normalizedCompiler = ToUpper(compilerString);

    if (normalizedCompiler == "CLANG") {
        return CLANG;
    } else if (normalizedCompiler == "GCC" || normalizedCompiler == "G++") {
        return GCC;
    } else if (normalizedCompiler == "MSVC") {
        return MSVC;
    }

    return DEFAULT;
}

Standard ParseStandardInfo(
    int standardValue
)
{
    switch (standardValue) {
        case 14:
            return S_14;
        case 17:
            return S_17;
        case 20:
            return S_20;
        case 23:
            return S_23;
        default:
            return S_26;
    }
}

std::vector<Dependency> ParseDependenciesInfo(
    const toml::table* dependenciesTable
)
{
    std::vector<Dependency> dependencies;

    if (dependenciesTable == nullptr) {
        return dependencies;
    }

    Dependency directDependency;
    bool hasDirectDependency = false;

    if (const auto* nameNode = (*dependenciesTable)["name"].as_string()) {
        directDependency.dependencyName = nameNode->get();
        hasDirectDependency = true;
    }

    if (const auto* urlNode = (*dependenciesTable)["git_url"].as_string()) {
        directDependency.gitUrl = urlNode->get();
        hasDirectDependency = true;
    }

    if (const auto* tagNode = (*dependenciesTable)["git_tag"].as_string()) {
        directDependency.gitTag = tagNode->get();
        hasDirectDependency = true;
    }

    if (hasDirectDependency) {
        dependencies.push_back(std::move(directDependency));
        return dependencies;
    }

    for (const auto& [_, dependencyNode] : *dependenciesTable) {
        const auto* dependencyTable = dependencyNode.as_table();
        if (dependencyTable == nullptr) {
            continue;
        }

        Dependency dependency;

        if (const auto* nameNode = (*dependencyTable)["name"].as_string()) {
            dependency.dependencyName = nameNode->get();
        }

        if (const auto* urlNode = (*dependencyTable)["git_url"].as_string()) {
            dependency.gitUrl = urlNode->get();
        }

        if (const auto* tagNode = (*dependencyTable)["git_tag"].as_string()) {
            dependency.gitTag = tagNode->get();
        }

        dependencies.push_back(std::move(dependency));
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
        values.projectLanguage = ParseProjectLanguageInfo(config["project"]["language"].value_or("C++"));
        values.compiler = ParseCompilerInfo(config["project"]["compiler"].value_or("default"));
        values.standard = ParseStandardInfo(config["project"]["standard"].value_or(S_26));
        values.Dependencies = ParseDependenciesInfo(config["dependencies"].as_table());
    } catch (const std::exception&) {
        return ConfigValues{};
    }

    return values;
}

std::string Helpers::SanitizeProjectName(
    const std::string& name
)
{
    std::string sanitized;
    sanitized.reserve(name.size());

    for (const unsigned char ch : name) {
        if (std::isalnum(ch) || ch == '_' || ch == '-' || ch == '.') {
            sanitized.push_back(static_cast<char>(ch));
        } else {
            sanitized.push_back('_');
        }
    }

    return sanitized.empty() ? "project" : sanitized;
}

void Helpers::PrintUsage() {
    std::cout << "Docked\n\n"
            << "Usage:\n"
            << "  docked build\n"
            << "  docked run\n"
            << "  docked init [path] [--name project-name]\n"
            << "  docked --help\n";
}