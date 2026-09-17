#ifndef BUILD_H
#define BUILD_H

#include "config.h"

class Build {
    public:
        Build() = delete;

        bool Execute(
            const ConfigValues& config
        );
        
    private:
        void GenerateNinjaFile(
          const ConfigValues& config
        );

        bool ExecuteNinja();

        std::string DetectDefaultCompiler();

        bool IsCompilerAvailable(
            const std::string& compiler
        );
    };

#endif