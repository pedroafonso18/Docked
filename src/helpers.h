#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>

class Helpers {
    public:
        Helpers() = delete;

        static std::string Trim(
            const std::string& value
        );
};

#endif