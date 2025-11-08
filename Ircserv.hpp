#pragma once

# include "Exceptions.hpp"
# include <iostream>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <string>
# include <vector>
# include "Token.hpp"

class Ircserv
{
    private:

        int _port; //0 to 65535
        std::string _password;

    public:
        Ircserv();
        Ircserv(int port, std::string pass);
        // Ircserv(const Ircserv &copy);
        // Ircserv &operator=(const Ircserv &other);
        ~Ircserv();


};   