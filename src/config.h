#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include "constants.h"

using namespace ConfigVariables;

typedef struct ConfigValues {
    std::string ProjectName;
    double ProjectVersion;
    ProjectLanguage ProjectLanguage;
    Compiler Compiler;
    Standard Standard;
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