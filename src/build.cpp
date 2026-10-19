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
        if (config.projectLanguage == ProjectLanguage::C)
        {
            switch (config.compiler)
            {
                case Compiler::GCC:
                    return "gcc";

                case Compiler::CLANG:
                    return "clang";

                case Compiler::MSVC:
                    return "cl";

                default:
                    return Build::DetectDefaultCompiler(ProjectLanguage::C);
            }
        }

        switch (config.compiler)
        {
            case Compiler::GCC:
                return "g++";

            case Compiler::CLANG:
                return "clang++";

            case Compiler::MSVC:
                return "cl";

            default:
                return Build::DetectDefaultCompiler(ProjectLanguage::CPP);
        }
    }

    std::string GetStandardFlag(
        const ConfigValues& config,
        const std::string& compiler
    )
    {
        if (config.projectLanguage == ProjectLanguage::C)
        {
            if (compiler == "cl")
            {
                switch (config.standard)
                {
                    case Standard::S_89:
                    case Standard::S_90:
                    case Standard::S_99:
                    case Standard::S_11:
                    case Standard::S_14:
                        return "/std:c11";

                    case Standard::S_17:
                    case Standard::S_20:
                        return "/std:c17";

                    case Standard::S_23:
                    case Standard::S_26:
                    default:
                        return "/std:clatest";
                }
            }

            switch (config.standard)
            {
                case Standard::S_89:
                    return "-std=c89";

                case Standard::S_90:
                    return "-std=c90";

                case Standard::S_99:
                    return "-std=c99";

                case Standard::S_11:
                case Standard::S_14:
                    return "-std=c11";

                case Standard::S_17:
                case Standard::S_20:
                    return "-std=c17";

                case Standard::S_23:
                case Standard::S_26:
                default:
                    return "-std=c23";
            }
        }

        if (compiler == "cl")
        {
            switch (config.standard)
            {
                case Standard::S_89:
                case Standard::S_90:
                case Standard::S_99:
                case Standard::S_11:
                case Standard::S_14:
                    return "/std:c++14";

                case Standard::S_17:
                    return "/std:c++17";

                case Standard::S_20:
                    return "/std:c++20";

                case Standard::S_23:
                case Standard::S_26:
                default:
                    return "/std:c++latest";
            }
        }

        switch (config.standard)
        {
            case Standard::S_89:
            case Standard::S_90:
            case Standard::S_99:
            case Standard::S_11:
                return "-std=c++11";

            case Standard::S_14:
                return "-std=c++14";

            case Standard::S_17:
                return "-std=c++17";

            case Standard::S_20:
                return "-std=c++20";

            case Standard::S_23:
                return "-std=c++23";

            case Standard::S_26:
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

        if (config.projectLanguage == ProjectLanguage::C)
        {
            ninjaFile << "cc = " << compiler << '\n';
            ninjaFile << "cflags = " << standard << '\n';
        }
        else
        {
            ninjaFile << "cxx = " << compiler << '\n';
            ninjaFile << "cxxflags = " << standard << '\n';
        }
        ninjaFile << '\n';
    }

    void WriteRules(
        std::ofstream& ninjaFile,
        const ConfigValues& config
    )
    {
        if (config.projectLanguage == ProjectLanguage::C)
        {
            ninjaFile << "rule cc\n";
            ninjaFile << "    command = $cc $cflags -MMD -MF $out.d -c $in -o $out\n";
            ninjaFile << "    depfile = $out.d\n";
            ninjaFile << "    deps = gcc\n";
            ninjaFile << '\n';

            ninjaFile << "rule link\n";
            ninjaFile << "    command = $cc $in -o $out\n";
            ninjaFile << '\n';
        }
        else
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
    }

    std::vector<std::string> WriteSourceBuilds(
        std::ofstream& ninjaFile,
        const ConfigValues& config
    )
    {
        std::vector<std::string> objectFiles;

        const fs::path sourceDirectory = SOURCE_DIRECTORY;
        const fs::path buildDirectory = BUILD_DIRECTORY;
        const fs::path objectDirectory =
            buildDirectory / OBJECT_DIRECTORY;

        if (!fs::exists(sourceDirectory))
            return objectFiles;

        const std::string targetExtension =
            (config.projectLanguage == ProjectLanguage::C) ? ".c" : ".cpp";
        const std::string ruleName =
            (config.projectLanguage == ProjectLanguage::C) ? "cc" : "cxx";

        for (const auto& entry :
             fs::recursive_directory_iterator(sourceDirectory))
        {
            if (!entry.is_regular_file())
                continue;

            const fs::path source = entry.path();

            if (source.extension() != targetExtension)
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
                << ": "
                << ruleName
                << ' '
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
    WriteRules(ninjaFile, config);

    const std::vector<std::string> objectFiles =
        WriteSourceBuilds(ninjaFile, config);

    WriteTarget(ninjaFile, config, objectFiles);
}

std::string Build::DetectDefaultCompiler(ProjectLanguage language)
{
    if (language == ProjectLanguage::C)
    {
#ifdef _WIN32
        if (IsCompilerAvailable("cl"))
            return "cl";

        if (IsCompilerAvailable("clang"))
            return "clang";

        if (IsCompilerAvailable("gcc"))
            return "gcc";

#elif defined(__linux__)
        if (IsCompilerAvailable("gcc"))
            return "gcc";

        if (IsCompilerAvailable("clang"))
            return "clang";

#elif defined(__APPLE__)
        if (IsCompilerAvailable("clang"))
            return "clang";

        if (IsCompilerAvailable("gcc"))
            return "gcc";

#endif
        throw NoAvailableCompiler();
    }

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