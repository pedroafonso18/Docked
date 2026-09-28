#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include "config.h"

class Dependencies {
    public:
        Dependencies() = delete;

        static void ResolveDependencies(
            const std::vector<Dependency> dependencies 
        );
};

#endif