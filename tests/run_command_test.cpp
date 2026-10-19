#include <gtest/gtest.h>

#include <filesystem>

#include "commands/build.h"
#include "commands/run.h"
#include "helpers.h"
#include "test_utils.h"

TEST(RunCommand, ExecutesBuiltBinaryFromBuildDirectory)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-run");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    const fs::path markerFile = tempRoot / "ran.txt";
#ifdef _WIN32
    const fs::path executable = tempRoot / "build" / "Sample_Project.exe";
    test_utils::WriteTextFile(
        executable,
        "@echo off\r\necho ran>\"" + markerFile.string() + "\"\r\n"
    );
#else
    const fs::path executable = tempRoot / "build" / "Sample_Project";
    test_utils::WriteTextFile(
        executable,
        "#!/bin/sh\n" \
        "echo ran > \"" + markerFile.string() + "\"\n"
    );
    test_utils::MakeExecutable(executable);
#endif

    ConfigValues config{};
    config.ProjectName = "Sample Project";

    Commands::ExecuteRunCommand(config);

    EXPECT_TRUE(fs::exists(markerFile));
}

TEST(RunCommand, ExecutesCompiledCBinary)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-run-c");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.c",
        "#include <stdio.h>\n"
        "int main(void) {\n"
        "    FILE *f = fopen(\"c_ran.txt\", \"w\");\n"
        "    if (f) {\n"
        "        fputs(\"ok\", f);\n"
        "        fclose(f);\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
    );

    ConfigValues config{};
    config.ProjectName = "c_run_test";
    config.projectLanguage = ProjectLanguage::C;
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;

    Commands::ExecuteBuildCommand(config);
    Commands::ExecuteRunCommand(config);

    EXPECT_TRUE(fs::exists(tempRoot / "c_ran.txt"));
}