#ifndef COMMANDS_HELP_H
#define COMMANDS_HELP_H

#include <boost/program_options.hpp>

namespace Commands {
    void CreateHelpCommand(
        boost::program_options::options_description& description
    );
}

#endif