#include "dependencies.h"

#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

void Dependencies::ResolveDependencies(
    const std::vector<Dependency> dependencies
)
{
    for (const Dependency dependency : dependencies)
    {
        const fs::path dependencyPath =
            fs::path(".docked") / "dependencies" / dependency.dependencyName;

        if (fs::exists(dependencyPath))
        {
            continue;
        }

        fs::create_directories(dependencyPath.parent_path());

        const std::string command =
            "git clone --branch " +
            dependency.gitTag +
            " --depth 1 " +
            dependency.gitUrl +
            " \"" +
            dependencyPath.string() +
            "\"";

        if (std::system(command.c_str()) != 0)
        {
            throw std::runtime_error(
                "Failed to clone dependency: " +
                dependency.dependencyName
            );
        }
    }
}