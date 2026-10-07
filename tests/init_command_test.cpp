#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "commands/init.h"
#include "helpers.h"
#include "test_utils.h"

TEST(InitCommand, ParsesPositionalPathAndName)
{
    const std::vector<std::string> arguments{ "./sample", "--name", "demo" };

    const Commands::InitCommandOptions options =
        Commands::ParseInitCommandArguments(arguments);

    EXPECT_EQ(options.projectPath, "./sample");
    ASSERT_TRUE(options.projectName.has_value());
    EXPECT_EQ(*options.projectName, "demo");
}

TEST(InitCommand, ExecuteCreatesProjectFilesFromTemplate)
{
    namespace fs = std::filesystem;

    const fs::path tempRoot = test_utils::MakeTempDirectory("docked-init");
    test_utils::ScopedCurrentPath currentPath(tempRoot);

    Commands::InitCommandOptions options{};
    options.projectPath = tempRoot.string();
    options.projectName = std::string("sample-app");

    Commands::ExecuteInitCommand(options);

    const fs::path projectDirectory = tempRoot / "sample-app";
    const fs::path configFile = projectDirectory / "docked.toml";
    const fs::path sourceFile = projectDirectory / "src" / "main.cpp";

    ASSERT_TRUE(fs::exists(configFile));
    ASSERT_TRUE(fs::exists(sourceFile));

    std::ifstream configStream(configFile);
    const std::string configContent{
        std::istreambuf_iterator<char>(configStream),
        std::istreambuf_iterator<char>()
    };

    EXPECT_NE(configContent.find("[project]"), std::string::npos);
    EXPECT_NE(configContent.find("name = \"Test Project\""), std::string::npos);
}