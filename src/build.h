#ifndef BUILD_H
#define BUILD_H

#include "config.h"

class Build {
    public:
        Build() = delete;

        static bool Execute(
            const ConfigValues& config
        );
        
    private:
        static void GenerateNinjaFile(
          const ConfigValues& config
        );

        static bool ExecuteNinja();

        static std::string DetectDefaultCompiler();

        static bool IsCompilerAvailable(
            const std::string& compiler
        );
    };

#endif