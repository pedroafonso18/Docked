#include "init.h"

#include <filesystem>
#include <fstream>

namespace Commands {

void CreateInitCommand(
    boost::program_options::options_description& description
)
{
    description.add_options()
        ("init", boost::program_options::value<std::string>()->default_value("."), "Project path")
        ("name", boost::program_options::value<std::string>(), "Project name");
}

void ExecuteInitCommand(
    const std::string& projectPath,
    const std::optional<std::string>& projectName,
    const ConfigValues& config
)
{
    namespace fs = std::filesystem;

    fs::path projectDirectory = projectPath;

    if (projectName.has_value()) {
        projectDirectory /= *projectName;
    }

    fs::create_directories(projectDirectory / "src");

    auto dockedConfigFile = std::ofstream(projectDirectory / "docked.toml");
    std::ofstream(projectDirectory / "src" / "main.cpp");

    std::ifstream exampleConfigFile("../docked.example.toml");
    if (exampleConfigFile.is_open()) {
        std::string line;
        bool skipDependencies = false;

        while (std::getline(exampleConfigFile, line)) {
            if (line.find("[dependencies]") != std::string::npos) {
                skipDependencies = true;
                continue;
            }

            if (skipDependencies) {
                continue;
            }

            dockedConfigFile << line << '\n';
        }
    }

    (void)config;
}

}