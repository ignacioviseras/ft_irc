#pragma once

#include "Ircserv.hpp"
#include <exception>



class Exceptions
{
    private:

    public:
        class ExceptionParams : public std::exception {
            public:
                const char *what() const throw();
        };
};
