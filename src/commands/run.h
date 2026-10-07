#ifndef COMMANDS_RUN_H
#define COMMANDS_RUN_H

#include <boost/program_options.hpp>
#include "../constants.h"

namespace Commands {
    void CreateRunCommand(
        boost::program_options::options_description& description
    );

    void ExecuteRunCommand(
        const ConfigValues& config
    );
};

#endif