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

InitCommandOptions ParseInitCommandArguments(
    const std::vector<std::string>& arguments
)
{
    namespace po = boost::program_options;

    po::options_description initDescription("Docked init");
    CreateInitCommand(initDescription);

    po::positional_options_description positional;
    positional.add("init", 1);

    po::variables_map variables;
    po::store(
        po::command_line_parser(arguments)
            .options(initDescription)
            .positional(positional)
            .run(),
        variables
    );
    po::notify(variables);

    InitCommandOptions options;
    options.projectPath = variables.count("init") ? variables["init"].as<std::string>() : ".";

    if (variables.count("name")) {
        options.projectName = variables["name"].as<std::string>();
    }

    return options;
}

void ExecuteInitCommand(
    const InitCommandOptions& options,
    const ConfigValues& config
)
{
    namespace fs = std::filesystem;

    fs::path projectDirectory = options.projectPath;
    std::string effectiveProjectName = options.projectName.has_value() ? *options.projectName : projectDirectory.filename().string();

    if (options.projectName.has_value()) {
        projectDirectory /= *options.projectName;
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

        while (std::getline(exampleConfigFile, line)) {
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