#include "../../include/Server.hpp"

void Server::_nick(int fd, const std::vector<std::string>& args) {
	Client& user = _clients[fd];
	if (args.size() < 2) {
		std::string errorMsg = ":irc.servidor.com 431 " + user.getNickname() + " :No nickname given";
		send_message(fd, errorMsg);
		//send_message(fd, "Error: NICK necesita el nickname 'NICK <nickname>'");
		return;
	}
	std::string nickName = args[1];
	
	if (nickName.empty()) {
		std::string errorMsg = ":irc.servidor.com 431 " + user.getNickname() + " :No nickname given";
		send_message(fd, errorMsg);
		//std::cout << "fd: " << fd << " error in setnickname" << std::endl;
		return;
	} else if (nicknameInUse(nickName)) {
		std::string errorMsg = ":irc.servidor.com 433 " + user.getNickname() + " " + nickName + " :Nickname is already in use";
		send_message(fd, errorMsg);
		//std::cout << "fd: " << fd << " error in setnickname" << std::endl;
		return;
	} else{
		user.setNickname(nickName);
		std::cout << "fd: " << fd << " setnickname '"<< nickName <<"' okey" << std::endl;
		checkRegistration(fd, user);
		return;
	} 
}
