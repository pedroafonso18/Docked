#include "help.h"

namespace Commands {

void CreateHelpCommand(
    boost::program_options::options_description& description
)
{
    description.add_options()
        ("help,h", "Comandos disponíveis");
}

} // namespace Commands