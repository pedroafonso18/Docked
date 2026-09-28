#include <filesystem>
#include <iostream>
#include <boost/program_options.hpp>
#include "commands/help.h"
#include "commands/init.h"
#include "commands/build.h"
#include "constants.h"
#include "helpers.h"

using namespace boost::program_options;

namespace {

void PrintUsage()
{
    std::cout << "Docked\n\n"
              << "Usage:\n"
              << "  docked build\n"
              << "  docked init [path] [--name project-name]\n"
              << "  docked --help\n";
}

ConfigValues LoadConfigForPath(
    const std::string& projectPath
)
{
    const auto configPath = std::filesystem::path(projectPath) / Constants::CONFIG_FILE_NAME;

    if (!std::filesystem::exists(configPath)) {
        return ConfigValues{};
    }

    return Helpers::ParseConfigFile(configPath.string());
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        PrintUsage();
        return 0;
    }

    const std::string command = argv[1];

    if (command == "--help" || command == "-h" || command == "help") {
        PrintUsage();
        return 0;
    }

    if (command == "build") {
        const ConfigValues buildConfig = LoadConfigForPath(".");
        Commands::ExecuteBuildCommand(buildConfig);
        return 0;
    }

    if (command == "init") {
        options_description initDescription("Docked init");
        Commands::CreateInitCommand(initDescription);

        positional_options_description positional;
        positional.add("init", 1);

        std::vector<std::string> forwardedArguments(argv + 2, argv + argc);
        variables_map vm;
        store(
            command_line_parser(forwardedArguments)
                .options(initDescription)
                .positional(positional)
                .run(),
            vm
        );
        notify(vm);

        const std::string path = vm.count("init") ? vm["init"].as<std::string>() : ".";

        std::optional<std::string> name;
        if (vm.count("name")) {
            name = vm["name"].as<std::string>();
        }

        Commands::ExecuteInitCommand(path, name, LoadConfigForPath(path));
        return 0;
    }

    std::cerr << "Unknown command: " << command << '\n';
    PrintUsage();
    return 1;
}
