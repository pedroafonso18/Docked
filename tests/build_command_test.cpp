#include <gtest/gtest.h>

#include <filesystem>

#include "build.h"
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

TEST(BuildCommand, ExecutesEndToEndAndProducesBinaryForC)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-build-c");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.c",
        "#include <stdio.h>\nint main(void) { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "Sample C Project";
    config.projectLanguage = ProjectLanguage::C;
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;

    Commands::ExecuteBuildCommand(config);

#ifdef _WIN32
    const fs::path executable = tempRoot / "build" / "Sample_C_Project.exe";
#else
    const fs::path executable = tempRoot / "build" / "Sample_C_Project";
#endif

    EXPECT_TRUE(fs::exists(tempRoot / "build" / "build.ninja"));
    EXPECT_TRUE(fs::exists(executable));

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("cc = "), std::string::npos);
    EXPECT_NE(ninjaContent.find("cflags = -std=c17"), std::string::npos);
    EXPECT_NE(ninjaContent.find("rule cc\n"), std::string::npos);
    EXPECT_NE(ninjaContent.find("command = $cc $cflags"), std::string::npos);
    EXPECT_NE(ninjaContent.find("command = $cc $in -o $out"), std::string::npos);
    EXPECT_NE(ninjaContent.find("build obj/main.o: cc ../src/main.c"), std::string::npos);
    EXPECT_NE(ninjaContent.find("build Sample_C_Project: link obj/main.o"), std::string::npos);
    EXPECT_EQ(ninjaContent.find("cxx"), std::string::npos);
}

TEST(BuildCommand, BuildsMultipleCSourceFiles)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-build-c-multi");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "utils" / "calc.h",
        "#ifndef CALC_H\n#define CALC_H\nint add(int a, int b);\n#endif\n"
    );

    test_utils::WriteTextFile(
        tempRoot / "src" / "utils" / "calc.c",
        "#include \"calc.h\"\nint add(int a, int b) { return a + b; }\n"
    );

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.c",
        "#include \"utils/calc.h\"\nint main(void) { return add(2, 3) == 5 ? 0 : 1; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "multi_c";
    config.projectLanguage = ProjectLanguage::C;
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_11;

    Commands::ExecuteBuildCommand(config);

#ifdef _WIN32
    const fs::path executable = tempRoot / "build" / "multi_c.exe";
#else
    const fs::path executable = tempRoot / "build" / "multi_c";
#endif

    EXPECT_TRUE(fs::exists(tempRoot / "build" / "build.ninja"));
    EXPECT_TRUE(fs::exists(executable));

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("cflags = -std=c11"), std::string::npos);
    EXPECT_NE(ninjaContent.find("build obj/main.o: cc ../src/main.c"), std::string::npos);
    EXPECT_NE(ninjaContent.find("build obj/utils/calc.o: cc ../src/utils/calc.c"), std::string::npos);
}

TEST(BuildCommand, GeneratesCStandardsFlags)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-build-c-std");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.c",
        "int main(void) { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "std_c";
    config.projectLanguage = ProjectLanguage::C;
    config.compiler = Compiler::GCC;
    config.standard = Standard::S_99;

    Commands::ExecuteBuildCommand(config);

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("cc = gcc"), std::string::npos);
    EXPECT_NE(ninjaContent.find("cflags = -std=c99"), std::string::npos);
}

TEST(BuildCommand, MsvcDefaultsToClosestCStandard)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-msvc-c-std");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.c",
        "int main(void) { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "msvc_c";
    config.projectLanguage = ProjectLanguage::C;
    config.compiler = Compiler::MSVC;
    config.standard = Standard::S_89;

    Commands::ExecuteBuildCommand(config);

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("cc = cl"), std::string::npos);
    EXPECT_NE(ninjaContent.find("cflags = /std:c11"), std::string::npos);
    EXPECT_EQ(ninjaContent.find("/std:clatest"), std::string::npos);
}

TEST(BuildCommand, DetectDefaultCompilerSupportsC)
{
    const std::string cCompiler = Build::DetectDefaultCompiler(ProjectLanguage::C);
    EXPECT_FALSE(cCompiler.empty());
#ifdef __linux__
    EXPECT_TRUE(cCompiler == "gcc" || cCompiler == "clang");
#endif

    const std::string cppCompiler = Build::DetectDefaultCompiler(ProjectLanguage::CPP);
    EXPECT_FALSE(cppCompiler.empty());
#ifdef __linux__
    EXPECT_TRUE(cppCompiler == "g++" || cppCompiler == "clang++");
#endif
}