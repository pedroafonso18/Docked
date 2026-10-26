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

TEST(BuildCommand, EmitsDefaultLibFlagForDependency)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-dep-flags");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.cpp",
        "int main() { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "sample_dep_app";
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;

    Dependency dep;
    dep.dependencyName = "samplelib";
    dep.linkFlags = { "-lextra" };
    dep.includeDirs = { "/sample/include" };
    config.Dependencies.push_back(dep);

    Commands::ExecuteBuildCommand(config);

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("-lsamplelib"), std::string::npos);
    EXPECT_NE(ninjaContent.find("-lextra"), std::string::npos);
    EXPECT_NE(ninjaContent.find("-I/sample/include"), std::string::npos);
}

TEST(BuildCommand, ParsesAndEmitsProjectLibsAndIncludeDirs)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-project-flags");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.cpp",
        "int main() { return 0; }\n"
    );

    ConfigValues config{};
    config.ProjectName = "custom_flags_app";
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;
    config.LinkFlags = { "-lcustomlib", "-L/custom/lib" };
    config.IncludeDirs = { "/custom/include" };

    Commands::ExecuteBuildCommand(config);

    std::ifstream ninjaStream(tempRoot / "build" / "build.ninja");
    const std::string ninjaContent{
        std::istreambuf_iterator<char>(ninjaStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(ninjaContent.find("-I/custom/include"), std::string::npos);
    EXPECT_NE(ninjaContent.find("-lcustomlib"), std::string::npos);
    EXPECT_NE(ninjaContent.find("-L/custom/lib"), std::string::npos);
}

TEST(BuildCommand, BuildsAndLinksLocalDependencyEndToEnd)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-local-dep");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    const fs::path depRoot = tempRoot / ".docked" / "dependencies" / "mathlib";

    test_utils::WriteTextFile(
        depRoot / "include" / "mathlib.h",
        "#ifndef MATHLIB_H\n"
        "#define MATHLIB_H\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif\n"
        "int math_add(int a, int b);\n"
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n"
        "#endif\n"
    );

    test_utils::WriteTextFile(
        depRoot / "src" / "math.c",
        "#include \"mathlib.h\"\nint math_add(int a, int b) { return a + b; }\n"
    );

    test_utils::WriteTextFile(
        depRoot / "CMakeLists.txt",
        "cmake_minimum_required(VERSION 3.15)\n"
        "project(mathlib C)\n"
        "add_library(mathlib STATIC src/math.c)\n"
        "target_include_directories(mathlib PUBLIC include)\n"
    );

    test_utils::WriteTextFile(
        tempRoot / "src" / "main.cpp",
        "#include <mathlib.h>\n"
        "int main() {\n"
        "    return math_add(10, 20) == 30 ? 0 : 1;\n"
        "}\n"
    );

    ConfigValues config{};
    config.ProjectName = "dep_test_app";
    config.compiler = Compiler::DEFAULT;
    config.standard = Standard::S_17;

    Dependency mathlibDep;
    mathlibDep.dependencyName = "mathlib";
    config.Dependencies.push_back(mathlibDep);

    Commands::ExecuteBuildCommand(config);

#ifdef _WIN32
    const fs::path executable = tempRoot / "build" / "dep_test_app.exe";
#else
    const fs::path executable = tempRoot / "build" / "dep_test_app";
#endif

    EXPECT_TRUE(fs::exists(executable));

    const int exitCode = std::system(executable.string().c_str());
    EXPECT_EQ(exitCode, 0);
}