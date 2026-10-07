#include <iostream>
#include "commands/help.h"
#include "commands/init.h"
#include "commands/build.h"
#include "commands/run.h"
#include "constants.h"
#include "helpers.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        Helpers::PrintUsage();
        return 0;
    }

    const std::string command = argv[1];

    if (command == "--help" || command == "-h" || command == "help") {
        Helpers::PrintUsage();
        return 0;
    }

    if (command == "build") {
        const ConfigValues buildConfig = Config::LoadConfigForPath(".");
        Commands::ExecuteBuildCommand(buildConfig);
        return 0;
    }

    if (command == "run") {
        const ConfigValues runConfig = Config::LoadConfigForPath(".");
        Commands::ExecuteRunCommand(runConfig);
        return 0;
    }

    if (command == "init") {
        std::vector<std::string> forwardedArguments(argv + 2, argv + argc);
        const Commands::InitCommandOptions initOptions = Commands::ParseInitCommandArguments(forwardedArguments);
        Commands::ExecuteInitCommand(initOptions, Config::LoadConfigForPath(initOptions.projectPath));
        return 0;
    }

    std::cerr << "Unknown command: " << command << '\n';
    Helpers::PrintUsage();
    return 1;
}
