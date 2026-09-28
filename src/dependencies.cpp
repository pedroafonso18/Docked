#include "dependencies.h"

#include <filesystem>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace {

std::string Quote(
    const std::string& value
)
{
    std::ostringstream stream;
    stream << std::quoted(value);
    return stream.str();
}

bool RunCommand(
    const std::string& command
)
{
    std::cout << command << '\n';
    return std::system(command.c_str()) == 0;
}

bool IsInstalledDependency(
    const fs::path& dependencyPath
)
{
    return fs::exists(dependencyPath / ".git");
}

} // namespace

void Dependencies::ResolveDependencies(
    const std::vector<Dependency> dependencies
)
{
    for (const Dependency dependency : dependencies)
    {
        const fs::path dependencyPath =
            fs::path(".docked") / "dependencies" / dependency.dependencyName;

        if (IsInstalledDependency(dependencyPath))
        {
            continue;
        }

        if (fs::exists(dependencyPath))
        {
            fs::remove_all(dependencyPath);
        }

        fs::create_directories(dependencyPath.parent_path());

        std::cout << "Resolving dependency: " << dependency.dependencyName << '\n';

        const std::string command =
            "git clone " +
            Quote(dependency.gitUrl) +
            " " +
            Quote(dependencyPath.string());

        if (!RunCommand(command))
        {
            fs::remove_all(dependencyPath);
            throw std::runtime_error(
                "Failed to clone dependency: " +
                dependency.dependencyName
            );
        }

        if (!dependency.gitTag.empty())
        {
            const std::string checkoutCommand =
                "git -C " +
                Quote(dependencyPath.string()) +
                " checkout " +
                Quote(dependency.gitTag);

            if (!RunCommand(checkoutCommand))
            {
                fs::remove_all(dependencyPath);
                throw std::runtime_error(
                    "Failed to checkout dependency ref: " +
                    dependency.dependencyName
                );
            }
        }
    }
}