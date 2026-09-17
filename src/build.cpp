#include "build.h"
#include "constants.h"
#include <filesystem>
#include <fstream>

bool Build::Execute(
    const ConfigValues& config
)
{
    GenerateNinjaFile(config);
    return ExecuteNinja();
}

void Build::GenerateNinjaFile(const ConfigValues& config)
{
    namespace fs = std::filesystem;

    const fs::path sourceDirectory = "src";
    const fs::path buildDirectory = "build";

    fs::create_directories(buildDirectory / "obj");

    std::ofstream ninjaFile(buildDirectory / "build.ninja");

    if (!ninjaFile)
        return;

    switch (config.Compiler)
    {
        case ConfigVariables::Compiler::GCC:
            ninjaFile << "cxx = g++\n";
            break;

        case ConfigVariables::Compiler::CLANG:
            ninjaFile << "cxx = clang++\n";
            break;

        case ConfigVariables::Compiler::MSVC:
            ninjaFile << "cxx = cl\n";
            break;

        default:
            ninjaFile << DetectDefaultCompiler();
            break;
    }

    ninjaFile << '\n';

    ninjaFile << "cxxflags = ";

    switch (config.Standard)
    {
        case ConfigVariables::Standard::FOURTEEN:
            ninjaFile << "-std=c++14";
            break;

        case ConfigVariables::Standard::SEVENTEEN:
            ninjaFile << "-std=c++17";
            break;

        case ConfigVariables::Standard::TWENTY:
            ninjaFile << "-std=c++20";
            break;

        case ConfigVariables::Standard::TWENTY_THREE:
            ninjaFile << "-std=c++23";
            break;

        case ConfigVariables::Standard::TWENTY_SIX:
            ninjaFile << "-std=c++26";
            break;

        default:
            break;
    }

    ninjaFile << '\n';
    ninjaFile << '\n';

    ninjaFile << "rule cxx\n";
    ninjaFile << "    command = $cxx $cxxflags -MMD -MF $out.d -c $in -o $out\n";
    ninjaFile << "    depfile = $out.d\n";
    ninjaFile << "    deps = gcc\n";
    ninjaFile << '\n';

    ninjaFile << "rule link\n";
    ninjaFile << "    command = $cxx $in -o $out\n";
    ninjaFile << '\n';

    std::vector<std::string> objectFiles;

    for (const auto& entry :
         fs::recursive_directory_iterator(sourceDirectory))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != ".cpp" || entry.path().extension() != ".c")
            continue;

        const fs::path relativeSource =
            fs::relative(entry.path(), sourceDirectory);

        const fs::path objectPath =
            buildDirectory / "obj" /
            relativeSource;

        fs::path objectFile = objectPath;
        objectFile.replace_extension(".o");

        fs::create_directories(objectFile.parent_path());

        const fs::path ninjaObject =
            fs::relative(objectFile, buildDirectory);

        const fs::path ninjaSource =
            fs::relative(entry.path(), buildDirectory);

        ninjaFile
            << "build "
            << ninjaObject.generic_string()
            << ": cxx "
            << ninjaSource.generic_string()
            << '\n';

        objectFiles.push_back(ninjaObject.generic_string());
    }

    ninjaFile << '\n';

    ninjaFile << "build "
              << config.ProjectName
              << ": link";

    for (const std::string& object : objectFiles)
    {
        ninjaFile << ' ' << object;
    }

    ninjaFile << '\n';
    ninjaFile << '\n';

    ninjaFile << "default "
              << config.ProjectName
              << '\n';
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

        throw; //Neste caso não tem o que fazer, o usuário não tem compilador.
}

bool Build::IsCompilerAvailable(
    const std::string& compiler
)
{
    #ifdef _WIN32
        const std::string command = "where " + compiler + " > nul 2>&1";
    #else
        const std::string command = "command -v " + compiler + " > /dev/null 2>&1";
    #endif

    return std::system(command.c_str()) == 0;
}

bool Build::ExecuteNinja()
{
    return std::system("ninja -C build") == 0;
}