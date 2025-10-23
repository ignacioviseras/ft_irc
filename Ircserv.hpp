#pragma once

#include "Exceptions.hpp"
#include <iostream>

class Ircserv
{
    private:

    public:
        Ircserv();
        Ircserv(const Ircserv &copy);
        Ircserv &operator=(const Ircserv &other);
        ~Ircserv();
};