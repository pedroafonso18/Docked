#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include "config.h"

#include <string>
#include <vector>

struct DependencyArtifacts {
    std::vector<std::string> includeDirectories;
    std::vector<std::string> staticLibraries;
    std::vector<std::string> linkFlags;
};

class Dependencies {
    public:
        Dependencies() = delete;

        static void ResolveDependencies(
            const std::vector<Dependency>& dependencies 
        );

        static void BuildDependencies(
            const std::vector<Dependency>& dependencies
        );

        static DependencyArtifacts CollectArtifacts(
            const std::vector<Dependency>& dependencies
        );
};

#endif