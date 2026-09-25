#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include "config.h"

class Helpers {
    public:
        Helpers() = delete;

        static std::string Trim(
            const std::string& value
        );

        static ConfigValues ParseConfigFile(
            const std::string& path
        );

        static std::string SanitizeProjectName(
            const std::string& name
        );
};

#endif