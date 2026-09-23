#include <filesystem>
#include <iostream>
#include <boost/program_options.hpp>
#include "commands/help.h"
#include "commands/init.h"
#include "commands/build.h"
#include "constants.h"
#include "helpers.h"

using namespace boost::program_options;

int main(int argc, char* argv[]) {
    options_description desc("Docked");

    Commands::CreateHelpCommand(desc);
    Commands::CreateInitCommand(desc);
    Commands::CreateBuildCommand(desc);

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);
    ConfigValues configValue = ConfigValues{};

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }

    if (vm.count("init")) {
        const std::string path = vm["init"].as<std::string>();

        std::optional<std::string> name;

        if (vm.count("name")) {
            name = vm["name"].as<std::string>();
        }

        std::filesystem::path projectDirectory = path;
        if (name.has_value()) {
            projectDirectory /= *name;
        }

        const auto configPath = projectDirectory / Constants::CONFIG_FILE_NAME;
        if (std::filesystem::exists(configPath)) {
            configValue = Helpers::ParseConfigFile(configPath.string());
        }

        Commands::ExecuteInitCommand(path, name, configValue);
    }

    if (vm.count("build")) {
        const std::string projectPath = vm.count("init") ? vm["init"].as<std::string>() : ".";
        const auto configPath = std::filesystem::path(projectPath) / Constants::CONFIG_FILE_NAME;
        const ConfigValues buildConfig = std::filesystem::exists(configPath)
            ? Helpers::ParseConfigFile(configPath.string())
            : ConfigValues{};

        Commands::ExecuteBuildCommand(buildConfig);
    }

    return 0;
}