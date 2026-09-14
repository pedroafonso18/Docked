#ifndef COMMANDS_INIT_H
#define COMMANDS_INIT_H

#include <optional>
#include <string>
#include <boost/program_options.hpp>

namespace Commands {
    void CreateInitCommand(
        boost::program_options::options_description& description
    );

    void ExecuteInitCommand(
        const std::string& projectPath,
        const std::optional<std::string>& projectName
    );
}

#endif