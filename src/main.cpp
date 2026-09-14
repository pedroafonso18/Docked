#include <iostream>
#include <boost/program_options.hpp>
#include "commands/help.h"
#include "commands/init.h"

using namespace boost::program_options;

int main(int argc, char* argv[]) {
    options_description desc("Docked");

    Commands::CreateHelpCommand(desc);
    Commands::CreateInitCommand(desc);

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 0;
    }
    if (vm.count("init")) {
        const std::string path = vm["path"].as<std::string>();

        std::optional<std::string> name;

        if (vm.count("name")) {
            name = vm["name"].as<std::string>();
        }

        Commands::ExecuteInitCommand(path, name);
    }
    return 0;
}