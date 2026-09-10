#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>

namespace Constants {
    const std::string CONFIG_FILE_NAME = "docked.toml";
};

namespace ConfigVariables {
    typedef enum ProjectLanguage {
        CPP,
        C
    };

    typedef enum Compiler {
        CLANG,
        GCC,
        MSVC
    };

    typedef enum Standard {
        FOURTEEN,
        SEVENTEEN,
        TWENTY,
        TWENTY_THREE,
        TWENTY_SIX
    };
};

#endif