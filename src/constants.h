#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <vector>

const std::string CONFIG_FILE_NAME = "docked.toml";

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

struct Dependency {
    std::string dependencyName;
    std::string gitUrl;
    std::string gitTag;
};

struct ConfigValues {
    std::string ProjectName;
    double ProjectVersion;
    ProjectLanguage ProjectLanguage;
    Compiler Compiler;
    Standard Standard;
    std::vector<Dependency> Dependencies;
};

#endif