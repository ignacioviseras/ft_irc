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
#include <fcntl.h>
#include "Channel.hpp"

class Server
{
    private:
        int _port; //0 to 65535
        std::string _password;
        bool _running;
        int _serverSocket;
        std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
        std::vector<pollfd> _pollfds;

        void setupServerSocket();
        void handleNewConnection();
        void handleClientData(int fd);
        void handleStdin();
        bool parse(const std::string& commandLine, std::vector<std::string>& args);
        void handleCommand(int fd, std::string& commandLine);
        void executeCommand(int fd, const std::vector<std::string>& args);
        bool nicknameInUse(const std::string& nick);
        void send_message(int fd, std::string message);
        void checkRegistration(int fd, Client &user);

		void _kickUser(Client* sender, const std::vector<std::string>& args);
		Client* findClientByNick(const std::string& nick);
		void sendToChannel(const Channel& channel, const std::string& msg);

		//mixi

		Channel* findServer(const std::vector<std::string>& args);


	public:
        Server(int port, const std::string &password);
        ~Server();
        void run();
};
