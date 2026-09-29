#ifndef ERRORS_H
#define ERRORS_H

#include <exception>

//-----------------------------------------------------------------------------//

class NoAvailableCompiler : public std::exception
{
    public:
        const char* what() const noexcept override
        {
            return "No available compiler found.";
        }
};

//-----------------------------------------------------------------------------//

#endif