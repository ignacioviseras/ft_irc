#include "../../include/Server.hpp"

void Server::_user(Client* sender, const std::vector<std::string>& args) {

	if (args.size() < 5) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " USER :Not enough parameters 'USER <username> 0 *:<realname>'";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	std::string username = args[1];
	std::string mode = args[2];
	std::string unused = args[3];
	if (mode != "0" || unused != "*"){
		std::string errorMsg = ":irc.servidor.com **** " + sender->getNickname() + " Error in params param_3 -> 0 and param_4 -> *";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	std::string realname;
	for (size_t i = 4; i < args.size(); ++i) {
		realname += args[i] + " ";
	}
	if (!realname.empty() && realname[0] == ':')
    	realname.erase(0, 1);
	sender->setUsername(username);
	sender->setMode(mode);
	sender->setUnused(unused);
	sender->setRealname(realname);
	std::cout << "fd: " << sender->getFd() << " setuser okey" << std::endl;
	checkRegistration(sender->getFd(), *sender);
}
