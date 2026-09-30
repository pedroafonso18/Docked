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

class NoNinjaFileCreated : public std::exception
{
    public:
        const char* what() const noexcept override
        {
            return "Couldn't create the ninja file.";
        }    
};

//-----------------------------------------------------------------------------//

#endif