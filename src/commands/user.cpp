#include "../../include/Server.hpp"

void Server::_user(Client* sender, const std::vector<std::string>& args) {
	if (args.size() != 5) {
		if (args.size() < 5) {
			std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " USER :Not enough parameters";
			send_message(sender->getFd(), errorMsg);
		}
		// casos como USER asd 0 * :"hola que tal" son validos en mi opinion
		// else {
		// 	std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " USER :Too many parameters";
		// 	send_message(sender->getFd(), errorMsg);
		// }
		return;
	}
	std::string username = args[1];
	std::string hostname = args[2];
	std::string servername = args[3];
	//no encuentro nada en rfc q me diga la necesida de obligar los campos 0 y * ejmpl(USER david 0 * :david)
	// if (hostname != "0" && servername != "*"){
	// 	std::string errorMsg = ":irc.servidor.com **** " + sender->getNickname() + "Error in params param 3 0 and param 4 *";
	// 	send_message(sender->getFd(), errorMsg);
	// }
	std::string realname;
	for (size_t i = 4; i < args.size(); ++i) {
		realname += args[i] + " ";
	}
	if (!realname.empty() && realname[0] == ':')
    	realname.erase(0, 1);
	sender->setUsername(username);
	sender->setHostname(hostname);
	sender->setServername(servername);
	sender->setRealname(realname);
	std::cout << "fd: " << sender->getFd() << " setuser okey" << std::endl;
	checkRegistration(sender->getFd(), *sender);
}
