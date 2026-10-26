#include "helpers.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include "../vendor/toml.h"

namespace {

std::vector<std::string> ParseStringList(
    const toml::node& node
)
{
    std::vector<std::string> result;
    if (const auto* str = node.as_string()) {
        std::istringstream stream(str->get());
        std::string token;
        while (stream >> token) {
            result.push_back(token);
        }
    } else if (const auto* arr = node.as_array()) {
        for (const auto& elem : *arr) {
            if (const auto* str = elem.as_string()) {
                result.push_back(str->get());
            }
        }
    }
    return result;
}

void AppendLibs(
    std::vector<std::string>& destination,
    const toml::node& node
)
{
    const auto items = ParseStringList(node);
    for (const auto& item : items) {
        if (item.empty()) continue;
        if (item[0] == '-') {
            destination.push_back(item);
        } else {
            destination.push_back("-l" + item);
        }
    }
}

void AppendLinkFlags(
    std::vector<std::string>& destination,
    const toml::node& node
)
{
    const auto items = ParseStringList(node);
    for (const auto& item : items) {
        if (!item.empty()) {
            destination.push_back(item);
        }
    }
}

void AppendIncludeDirs(
    std::vector<std::string>& destination,
    const toml::node& node
)
{
    const auto items = ParseStringList(node);
    for (const auto& item : items) {
        if (!item.empty()) {
            destination.push_back(item);
        }
    }
}

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

    if (normalizedCompiler == "CLANG" || normalizedCompiler == "CLANG++") {
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
        case 89:
            return S_89;
        case 90:
            return S_90;
        case 99:
            return S_99;
        case 11:
            return S_11;
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

    if (const auto* libsNode = (*dependenciesTable)["libs"].node()) {
        AppendLibs(directDependency.linkFlags, *libsNode);
        hasDirectDependency = true;
    }

    if (const auto* flagsNode = (*dependenciesTable)["link_flags"].node()) {
        AppendLinkFlags(directDependency.linkFlags, *flagsNode);
        hasDirectDependency = true;
    }

    if (const auto* incNode = (*dependenciesTable)["include_dirs"].node()) {
        AppendIncludeDirs(directDependency.includeDirs, *incNode);
        hasDirectDependency = true;
    }

    if (hasDirectDependency) {
        dependencies.push_back(std::move(directDependency));
        return dependencies;
    }

    for (const auto& [key, dependencyNode] : *dependenciesTable) {
        const auto* dependencyTable = dependencyNode.as_table();
        if (dependencyTable == nullptr) {
            continue;
        }

        Dependency dependency;

        if (const auto* nameNode = (*dependencyTable)["name"].as_string()) {
            dependency.dependencyName = nameNode->get();
        } else {
            dependency.dependencyName = std::string(key.str());
        }

        if (const auto* urlNode = (*dependencyTable)["git_url"].as_string()) {
            dependency.gitUrl = urlNode->get();
        }

        if (const auto* tagNode = (*dependencyTable)["git_tag"].as_string()) {
            dependency.gitTag = tagNode->get();
        }

        if (const auto* libsNode = (*dependencyTable)["libs"].node()) {
            AppendLibs(dependency.linkFlags, *libsNode);
        }

        if (const auto* flagsNode = (*dependencyTable)["link_flags"].node()) {
            AppendLinkFlags(dependency.linkFlags, *flagsNode);
        }

        if (const auto* incNode = (*dependencyTable)["include_dirs"].node()) {
            AppendIncludeDirs(dependency.includeDirs, *incNode);
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
        if (const auto* libsNode = config["project"]["libs"].node()) {
            AppendLibs(values.LinkFlags, *libsNode);
        }
        if (const auto* linkFlagsNode = config["project"]["link_flags"].node()) {
            AppendLinkFlags(values.LinkFlags, *linkFlagsNode);
        }
        if (const auto* incNode = config["project"]["include_dirs"].node()) {
            AppendIncludeDirs(values.IncludeDirs, *incNode);
        }
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