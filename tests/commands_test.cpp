#include <gtest/gtest.h>

#include <boost/program_options.hpp>
#include <sstream>

#include "commands/build.h"
#include "commands/help.h"
#include "commands/init.h"
#include "commands/run.h"

TEST(CommandRegistration, HelpCommandAddsOption)
{
    boost::program_options::options_description description("help");

    Commands::CreateHelpCommand(description);

    std::ostringstream stream;
    stream << description;
    const std::string output = stream.str();

    EXPECT_NE(output.find("help"), std::string::npos);
}

TEST(CommandRegistration, BuildCommandAddsOption)
{
    boost::program_options::options_description description("build");

    Commands::CreateBuildCommand(description);

    std::ostringstream stream;
    stream << description;
    const std::string output = stream.str();

    EXPECT_NE(output.find("build"), std::string::npos);
}

TEST(CommandRegistration, RunCommandAddsOption)
{
    boost::program_options::options_description description("run");

    Commands::CreateRunCommand(description);

    std::ostringstream stream;
    stream << description;
    const std::string output = stream.str();

    EXPECT_NE(output.find("run"), std::string::npos);
}

TEST(CommandRegistration, InitCommandAddsOptions)
{
    boost::program_options::options_description description("init");

    Commands::CreateInitCommand(description);

    std::ostringstream stream;
    stream << description;
    const std::string output = stream.str();

    EXPECT_NE(output.find("init"), std::string::npos);
    EXPECT_NE(output.find("name"), std::string::npos);
}