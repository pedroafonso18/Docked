#include "build.h"

#include "constants.h"
#include "dependencies.h"
#include "errors.h"
#include "helpers.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
    namespace fs = std::filesystem;

    constexpr const char* SOURCE_DIRECTORY = "src";
    constexpr const char* BUILD_DIRECTORY = "build";
    constexpr const char* OBJECT_DIRECTORY = "obj";

    std::string GetCompiler(const ConfigValues& config)
    {
        switch (config.compiler)
        {
            case Compiler::GCC:
                return "g++";

            case Compiler::CLANG:
                return "clang++";

            case Compiler::MSVC:
                return "cl";

            default:
                return Build::DetectDefaultCompiler();
        }
    }

    std::string GetStandardFlag(
        const ConfigValues& config,
        const std::string& compiler
    )
    {
        if (compiler == "cl")
        {
            switch (config.standard)
            {
                case Standard::S_14:
                    return "/std:c++14";

                case Standard::S_17:
                    return "/std:c++17";

                case Standard::S_20:
                    return "/std:c++20";

                case Standard::S_23:
                    return "/std:c++latest";

                default:
                    return "/std:c++latest";
            }
        }

        switch (config.standard)
        {
            case Standard::S_14:
                return "-std=c++14";

            case Standard::S_17:
                return "-std=c++17";

            case Standard::S_20:
                return "-std=c++20";

            case Standard::S_23:
                return "-std=c++23";

            default:
                return "-std=c++26";
        }
    }

    void WriteCompilerConfiguration(
        std::ofstream& ninjaFile,
        const ConfigValues& config
    )
    {
        const std::string compiler = GetCompiler(config);
        const std::string standard = GetStandardFlag(config, compiler);

        ninjaFile << "cxx = " << compiler << '\n';
        ninjaFile << "cxxflags = " << standard << '\n';
        ninjaFile << '\n';
    }

    void WriteRules(
        std::ofstream& ninjaFile
    )
    {
        ninjaFile << "rule cxx\n";
        ninjaFile << "    command = $cxx $cxxflags -MMD -MF $out.d -c $in -o $out\n";
        ninjaFile << "    depfile = $out.d\n";
        ninjaFile << "    deps = gcc\n";
        ninjaFile << '\n';

        ninjaFile << "rule link\n";
        ninjaFile << "    command = $cxx $in -o $out\n";
        ninjaFile << '\n';
    }

    std::vector<std::string> WriteSourceBuilds(
        std::ofstream& ninjaFile
    )
    {
        std::vector<std::string> objectFiles;

        const fs::path sourceDirectory = SOURCE_DIRECTORY;
        const fs::path buildDirectory = BUILD_DIRECTORY;
        const fs::path objectDirectory =
            buildDirectory / OBJECT_DIRECTORY;

        if (!fs::exists(sourceDirectory))
            return objectFiles;

        for (const auto& entry :
             fs::recursive_directory_iterator(sourceDirectory))
        {
            if (!entry.is_regular_file())
                continue;

            const fs::path source = entry.path();

            if (source.extension() != ".cpp")
                continue;

            const fs::path relativeSource =
                fs::relative(source, sourceDirectory);

            fs::path object =
                objectDirectory / relativeSource;

            object.replace_extension(".o");

            fs::create_directories(object.parent_path());

            const std::string ninjaObject =
                fs::relative(object, buildDirectory).generic_string();

            const std::string ninjaSource =
                fs::relative(source, buildDirectory).generic_string();

            ninjaFile
                << "build "
                << ninjaObject
                << ": cxx "
                << ninjaSource
                << '\n';

            objectFiles.push_back(ninjaObject);
        }

        return objectFiles;
    }

    void WriteTarget(
        std::ofstream& ninjaFile,
        const ConfigValues& config,
        const std::vector<std::string>& objectFiles
    )
    {
        const std::string projectName =
            Helpers::SanitizeProjectName(
                config.ProjectName.empty()
                    ? "project"
                    : config.ProjectName
            );

        ninjaFile << "build " << projectName << ": link";

        for (const std::string& object : objectFiles)
            ninjaFile << ' ' << object;

        ninjaFile << '\n';
        ninjaFile << '\n';
        ninjaFile << "default " << projectName << '\n';
    }
}

bool Build::Execute(const ConfigValues& config)
{
    Dependencies::ResolveDependencies(config.Dependencies);

    GenerateNinjaFile(config);

    return ExecuteNinja();
}

void Build::GenerateNinjaFile(const ConfigValues& config)
{
    const std::filesystem::path buildDirectory = BUILD_DIRECTORY;

    std::filesystem::create_directories(
        buildDirectory / OBJECT_DIRECTORY
    );

    std::ofstream ninjaFile(
        buildDirectory / "build.ninja"
    );

    if (!ninjaFile)
        throw NoNinjaFileCreated();

    WriteCompilerConfiguration(ninjaFile, config);
    WriteRules(ninjaFile);

    const std::vector<std::string> objectFiles =
        WriteSourceBuilds(ninjaFile);

    WriteTarget(ninjaFile, config, objectFiles);
}

std::string Build::DetectDefaultCompiler()
{
#ifdef _WIN32

    if (IsCompilerAvailable("cl"))
        return "cl";

    if (IsCompilerAvailable("clang++"))
        return "clang++";

    if (IsCompilerAvailable("g++"))
        return "g++";

#elif defined(__linux__)

    if (IsCompilerAvailable("g++"))
        return "g++";

    if (IsCompilerAvailable("clang++"))
        return "clang++";

#elif defined(__APPLE__)

    if (IsCompilerAvailable("clang++"))
        return "clang++";

    if (IsCompilerAvailable("g++"))
        return "g++";

#endif

    throw NoAvailableCompiler();
}

bool Build::IsCompilerAvailable(const std::string& compiler)
{
#ifdef _WIN32
    const std::string command =
        "where " + compiler + " > nul 2>&1";
#else
    const std::string command =
        "command -v " + compiler + " > /dev/null 2>&1";
#endif

    return std::system(command.c_str()) == 0;
}

bool Build::ExecuteNinja()
{
    return std::system("ninja -C build") == 0;
}