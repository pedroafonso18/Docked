#ifndef COMMANDS_INIT_H
#define COMMANDS_INIT_H

#include <optional>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include "../config.h"

namespace Commands {
    struct InitCommandOptions {
        std::string projectPath;
        std::optional<std::string> projectName;
    };

    void CreateInitCommand(
        boost::program_options::options_description& description
    );

    InitCommandOptions ParseInitCommandArguments(
        const std::vector<std::string>& arguments
    );

    void ExecuteInitCommand(
        const InitCommandOptions& options,
        const ConfigValues& config = ConfigValues{}
    );
}

#endif