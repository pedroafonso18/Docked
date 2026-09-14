#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include "constants.h"

struct ConfigValues {
    std::string ProjectName;
    double ProjectVersion;
    ConfigVariables::ProjectLanguage ProjectLanguage;
    ConfigVariables::Compiler Compiler;
    ConfigVariables::Standard Standard;
    std::vector<std::string> Dependencies;
};

class Config {
    public:
        Config(
            const std::string& projectPath
        );
    
        ~Config() = default;

        ConfigValues GetValues();

    private:
        ConfigValues values;
};

#endif 