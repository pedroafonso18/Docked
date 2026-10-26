#include "dependencies.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "helpers.h"

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

std::string ExecuteCommandCaptureOutput(
    const std::string& command
)
{
    std::string result;
    FILE* pipe =
#ifdef _WIN32
        _popen(command.c_str(), "r");
#else
        popen(command.c_str(), "r");
#endif
    if (!pipe) {
        return "";
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    return Helpers::Trim(result);
}

bool HasPkgConfig(
    const std::string& name
)
{
#ifdef _WIN32
    const std::string cmd = "pkg-config --exists " + Quote(name) + " > nul 2>&1";
#else
    const std::string cmd = "pkg-config --exists " + Quote(name) + " > /dev/null 2>&1";
#endif
    return std::system(cmd.c_str()) == 0;
}

void AddUnique(
    std::vector<std::string>& destination,
    const std::string& item
)
{
    if (item.empty()) {
        return;
    }
    if (std::find(destination.begin(), destination.end(), item) == destination.end()) {
        destination.push_back(item);
    }
}

bool IsInstalledDependency(
    const fs::path& dependencyPath
)
{
    return fs::exists(dependencyPath / ".git");
}

bool ContainsHeaderFiles(
    const fs::path& directory
)
{
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return false;
    }

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string ext = entry.path().extension().string();
        if (ext == ".h" || ext == ".hpp" || ext == ".hxx" || ext == ".hh") {
            return true;
        }
    }

    return false;
}

bool IsDependencyBuilt(
    const fs::path& dependencyPath
)
{
    const fs::path buildPath = dependencyPath / "build";
    if (!fs::exists(buildPath)) {
        return false;
    }

    for (const auto& entry : fs::recursive_directory_iterator(buildPath)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string ext = entry.path().extension().string();
        if (ext == ".a" || ext == ".lib" || ext == ".so" || ext == ".dylib" || ext == ".dll") {
            return true;
        }
    }

    return false;
}

void CollectIncludeDirsFromPath(
    const fs::path& depPath,
    std::vector<std::string>& includeDirs
)
{
    if (!fs::exists(depPath)) {
        return;
    }

    if (fs::exists(depPath / "include")) {
        AddUnique(includeDirs, (depPath / "include").lexically_normal().string());
    }

    if (fs::exists(depPath / "src") && ContainsHeaderFiles(depPath / "src")) {
        AddUnique(includeDirs, (depPath / "src").lexically_normal().string());
    }

    if (ContainsHeaderFiles(depPath)) {
        AddUnique(includeDirs, depPath.lexically_normal().string());
    }

    for (const auto& entry : fs::directory_iterator(depPath)) {
        if (entry.is_directory()) {
            if (fs::exists(entry.path() / "include")) {
                AddUnique(includeDirs, (entry.path() / "include").lexically_normal().string());
            }
        }
    }
}

void CollectStaticLibrariesFromPath(
    const fs::path& depPath,
    std::vector<std::string>& staticLibs
)
{
    if (!fs::exists(depPath)) {
        return;
    }

    const fs::path buildPath = depPath / "build";
    const fs::path searchPath = fs::exists(buildPath) ? buildPath : depPath;

    for (const auto& entry : fs::recursive_directory_iterator(searchPath)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::string ext = entry.path().extension().string();
        if (ext == ".a" || ext == ".lib") {
            AddUnique(staticLibs, entry.path().lexically_normal().string());
        }
    }
}

} // namespace

void Dependencies::ResolveDependencies(
    const std::vector<Dependency>& dependencies
)
{
    for (const Dependency& dependency : dependencies)
    {
        if (dependency.gitUrl.empty())
        {
            continue;
        }

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

void Dependencies::BuildDependencies(
    const std::vector<Dependency>& dependencies
)
{
    for (const Dependency& dependency : dependencies)
    {
        const fs::path dependencyPath =
            fs::path(".docked") / "dependencies" / dependency.dependencyName;

        if (!fs::exists(dependencyPath))
        {
            continue;
        }

        if (IsDependencyBuilt(dependencyPath))
        {
            continue;
        }

        if (fs::exists(dependencyPath / "CMakeLists.txt"))
        {
            std::cout << "Building dependency: " << dependency.dependencyName << '\n';

            fs::create_directories(dependencyPath / "build");

            const std::string cmakeConfigure =
                "cmake -B " + Quote((dependencyPath / "build").string()) +
                " -S " + Quote(dependencyPath.string()) +
                " -DCMAKE_BUILD_TYPE=Release" +
                " -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF";

            if (RunCommand(cmakeConfigure))
            {
                const std::string cmakeBuild =
                    "cmake --build " + Quote((dependencyPath / "build").string());
                RunCommand(cmakeBuild);
            }
        }
        else if (fs::exists(dependencyPath / "docked.toml"))
        {
            std::cout << "Building docked dependency: " << dependency.dependencyName << '\n';
            RunCommand("docked build --path " + Quote(dependencyPath.string()));
        }
        else if (fs::exists(dependencyPath / "Makefile"))
        {
            std::cout << "Building dependency with make: " << dependency.dependencyName << '\n';
            RunCommand("make -C " + Quote(dependencyPath.string()));
        }
    }
}

DependencyArtifacts Dependencies::CollectArtifacts(
    const std::vector<Dependency>& dependencies
)
{
    DependencyArtifacts artifacts;

    for (const Dependency& dependency : dependencies)
    {
        const fs::path dependencyPath =
            fs::path(".docked") / "dependencies" / dependency.dependencyName;

        CollectIncludeDirsFromPath(dependencyPath, artifacts.includeDirectories);

        for (const auto& inc : dependency.includeDirs)
        {
            AddUnique(artifacts.includeDirectories, inc);
        }

        CollectStaticLibrariesFromPath(dependencyPath, artifacts.staticLibraries);

        for (const auto& flag : dependency.linkFlags)
        {
            AddUnique(artifacts.linkFlags, flag);
        }

        if (HasPkgConfig(dependency.dependencyName))
        {
            const std::string cflags =
                ExecuteCommandCaptureOutput("pkg-config --cflags " + Quote(dependency.dependencyName));
            std::istringstream cflagsStream(cflags);
            std::string flag;
            while (cflagsStream >> flag)
            {
                if (flag.rfind("-I", 0) == 0 && flag.size() > 2)
                {
                    AddUnique(artifacts.includeDirectories, flag.substr(2));
                }
            }

            const std::string libs =
                ExecuteCommandCaptureOutput("pkg-config --libs " + Quote(dependency.dependencyName));
            std::istringstream libsStream(libs);
            while (libsStream >> flag)
            {
                AddUnique(artifacts.linkFlags, flag);
            }
        }

        if (artifacts.staticLibraries.empty())
        {
            const std::string defaultLibFlag = "-l" + dependency.dependencyName;
            bool hasLibFlag = false;
            for (const auto& f : artifacts.linkFlags)
            {
                if (f == defaultLibFlag || f.find("lib" + dependency.dependencyName) != std::string::npos)
                {
                    hasLibFlag = true;
                    break;
                }
            }
            if (!hasLibFlag)
            {
                AddUnique(artifacts.linkFlags, defaultLibFlag);
            }
        }
    }

    return artifacts;
}