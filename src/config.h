#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <vector>
#include "constants.h"

class Config {
    public:
        explicit Config(
            const std::string& projectPath
        );
    
        ~Config() = default;

        ConfigValues GetValues();

    private:
        ConfigValues values;
};

#endif 