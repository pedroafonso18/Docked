#include "run.h"

#include <cstdlib>
#include <filesystem>
#include "../helpers.h"

namespace Commands {

    void CreateRunCommand(
        boost::program_options::options_description& description
    )
    {
        description.add_options()
            ("run", "Roda o código");
    }

    void ExecuteRunCommand(
        const ConfigValues& config
    )
    {
        namespace fs = std::filesystem;

        const std::string projectName =
            Helpers::SanitizeProjectName(
                config.ProjectName.empty() ? "Test Project" : config.ProjectName
            );

#ifdef _WIN32
        const fs::path executable = fs::path("build") / (projectName + ".exe");
#else
        const fs::path executable = fs::path("build") / projectName;
#endif

        if (!fs::exists(executable)) {
            return;
        }

        std::system(('"' + executable.string() + '"').c_str());
    }
};