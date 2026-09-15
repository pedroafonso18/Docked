#ifndef COMMANDS_BUILD_H
#define COMMANDS_BUILD_H

#include <optional>
#include <string>
#include <boost/program_options.hpp>
#include "../config.h"

namespace Commands {
    void CreateBuildCommand(
        boost::program_options::options_description& description
    );

    void ExecuteBuildCommand(
        const std::string& projectPath,
        const ConfigValues& config
    );
}

#endif