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
    S_89,
    S_90,
    S_99,
    S_11,
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
    std::vector<std::string> linkFlags;
    std::vector<std::string> includeDirs;
};

struct ConfigValues {
    std::string ProjectName;
    double ProjectVersion;
    ProjectLanguage projectLanguage;
    Compiler compiler;
    Standard standard;
    std::vector<std::string> LinkFlags;
    std::vector<std::string> IncludeDirs;
    std::vector<Dependency> Dependencies;
};

#endif