#include "build.h"

namespace Commands {

    void CreateBuildCommand(
        boost::program_options::options_description& description
    )
    {
        description.add_options()
            ("build", "Compiles and builds the project");
    }

    void ExecuteBuildCommand(
        const std::string& projectPath,
        const ConfigValues& config
    )
    {
        
    }
}