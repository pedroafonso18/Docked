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
        FOURTEEN,
        SEVENTEEN,
        TWENTY,
        TWENTY_THREE,
        TWENTY_SIX
    };
}

#endif