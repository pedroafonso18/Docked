#include <iostream>
#include <boost/program_options.hpp>
#include "commands/help.h"

using namespace boost::program_options;

int main(int argc, char* argv[]) {
    options_description desc("Docked");

    Commands::CreateHelpCommand(desc);

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
    return 0;
}