#pragma once

# include "Exceptions.hpp"
# include "Client.hpp"
# include "Token.hpp"
# include <iostream>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <string>
# include <vector>
# include <map>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include "Utils.hpp"
#include <fcntl.h>
#include "Channel.hpp"
#include <sys/epoll.h>

class Server
{
    private:
        int _port; //0 to 65535
        std::string _password;
        bool _running;
        int _serverSocket;
        std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;
        int _epollFd;
        struct epoll_event _events[64];
        void handleClientWrite(int fd);

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

		
		void _join(int fd, const std::vector<std::string>& args);
		void _privMsg(Client* sender, const std::vector<std::string>& args);
		void _kick(Client* sender, const std::vector<std::string>& args);
        void _names(int fd, const std::vector<std::string>& args);
        void sendNames(int fd, std::string serverName, std::string nickname, std::string chanName, std::string userList);
		
		void _user(Client* sender, const std::vector<std::string>& args);
		void _quit(Client* sender, const std::vector<std::string>& args);
		void _pass(Client* sender, const std::vector<std::string>& args);
		void _part(Client* sender, const std::vector<std::string>& args);
		void _nick(int fd, const std::vector<std::string>& args);
		
		void sendToChannel(const Channel& channel, const std::string& msg);
		
		Client* findClientByNick(const std::string& nick);
		void disconnectClient(int fd);
		
		//mixi
		
		Channel* findServer(const std::vector<std::string>& args);
		void	commandList(int fd);


	public:
        Server(int port, const std::string &password);
        ~Server();
        void run();
};
