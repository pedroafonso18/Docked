#include <gtest/gtest.h>

#include <filesystem>

#include "commands/build.h"
#include "constants.h"
#include "test_utils.h"

TEST(BuildCommand, ExecutesEndToEndAndProducesBinary)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-build");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.cpp",
        "int main() { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "Sample Project";
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;

    Commands::ExecuteBuildCommand(config);

#ifdef _WIN32
    const fs::path executable = tempRoot / "build" / "Sample_Project.exe";
#else
    const fs::path executable = tempRoot / "build" / "Sample_Project";
#endif

    EXPECT_TRUE(fs::exists(tempRoot / "build" / "build.ninja"));
    EXPECT_TRUE(fs::exists(executable));
}