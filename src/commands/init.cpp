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
    std::string effectiveProjectName = projectName.has_value() ? *projectName : projectDirectory.filename().string();

    if (projectName.has_value()) {
        projectDirectory /= *projectName;
    }

    fs::create_directories(projectDirectory / "src");

    auto dockedConfigFile = std::ofstream(projectDirectory / "docked.toml");
    std::ofstream mainFile(projectDirectory / "src" / "main.cpp");
    if (mainFile.is_open()) {
        mainFile << "int main() {\n"
                 << "    return 0;\n"
                 << "}\n";
    }

    const fs::path templatePath = fs::path(__FILE__).parent_path().parent_path().parent_path() / "docked.example.toml";
    std::ifstream exampleConfigFile(templatePath);
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
    } else {
        dockedConfigFile
            << "[project]\n"
            << "name = \"" << effectiveProjectName << "\"\n"
            << "version = 1.0\n"
            << "language = \"C++\"\n"
            << "compiler = \"default\"\n"
            << "standard = 17\n\n"
            << "[dependencies]\n";
    }

    (void)config;
}

}