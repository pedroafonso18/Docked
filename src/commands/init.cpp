#include "init.h"

#include <filesystem>
#include <fstream>

namespace Commands {

void CreateInitCommand(
    boost::program_options::options_description& description
)
{
    description.add_options()
        ("init", "Create a new Docked project")
        ("name", boost::program_options::value<std::string>(), "Project name");
}

void ExecuteInitCommand(
    const std::string& projectPath,
    const std::optional<std::string>& projectName
)
{
    namespace fs = std::filesystem;

    fs::path projectDirectory = projectPath;

    if (projectName.has_value()) {
        projectDirectory /= *projectName;
    }

    fs::create_directories(projectDirectory / "src");

    std::ofstream(projectDirectory / "docked.toml");
    std::ofstream(projectDirectory / "src" / "main.cpp");
}

}