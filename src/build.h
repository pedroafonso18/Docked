#ifndef BUILD_H
#define BUILD_H

#include "config.h"

class Build
{
    public:
        Build() = delete;

        static bool Execute(const ConfigValues& config);

        static std::string DetectDefaultCompiler();
    private:
        static void GenerateNinjaFile(const ConfigValues& config);
        static bool ExecuteNinja();
        static bool IsCompilerAvailable(
            const std::string& compiler
        );
};

#endif