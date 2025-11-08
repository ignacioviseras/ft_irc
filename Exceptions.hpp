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
        class ExceptionTest : public std::exception {
            public:
                const char *what() const throw();
        };
        class NonValidCommandException : public std::exception {
            public:
                const char *what() const throw();
        };
        class NonValidPortException : public std::exception {
            public:
                const char *what() const throw();
        };
        
};
