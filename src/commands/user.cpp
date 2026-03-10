#include "../../include/Server.hpp"

void Server::_user(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 5) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " USER :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		//send_message(sender->getFd(), "Error: USER necesita 4 parámetros 'USER <username> <hostname> <servername> :<realname>'");
		return;
	}
	std::string username = args[1];
	std::string hostname = args[2];
	std::string servername = args[3];
	std::string realname;
	for (size_t i = 4; i < args.size(); ++i) {
		realname += args[i] + " ";
	}
	if (!realname.empty())
		realname.erase(realname.length() - 1);
	sender->setUsername(username);
	sender->setHostname(hostname);
	sender->setServername(servername);
	sender->setRealname(realname);
	std::cout << "fd: " << sender->getFd() << " setuser okey" << std::endl;
	checkRegistration(sender->getFd(), *sender);
}
