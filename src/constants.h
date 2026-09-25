#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

namespace Constants {
    const std::string CONFIG_FILE_NAME = "docked.toml";
};

namespace ConfigVariables {
    enum ProjectLanguage {
        CPP,
        C
    };

    enum Compiler {
        CLANG,
        GCC,
        MSVC,
        DEFAULT
    };

    enum Standard {
        S_14,
        S_17,
        S_20,
        S_23,
        S_26
    };
}

#endif