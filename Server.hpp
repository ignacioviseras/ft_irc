#pragma once

# include "Exceptions.hpp"
# include "Client.hpp"
# include "Token.hpp"
# include <iostream>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <string>
# include <poll.h>
# include <vector>
# include <map>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include "Utils.hpp"

class Server
{
    private:
        int _port; //0 to 65535
        std::string _password;
        bool _running;
        int _serverSocket;
        std::map<int, Client> _clients;
        std::vector<pollfd> _pollfds;

        void setupServerSocket();
        void handleNewConnection();
        void handleClientData(int fd);
        void handleStdin();
    public:
        Server(int port, const std::string &pass);
        ~Server();
        void run();

};   