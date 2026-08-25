#include "help.h"

void CreateHelpCommand(
    boost::program_options::options_description& description
) 
{
    description.add_options()
        ("help,h", "Comandos disponíveis");
}