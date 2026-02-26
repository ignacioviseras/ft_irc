#include "../../include/Server.hpp"

void Server::_nick(int fd, const std::vector<std::string>& args) {
	Client& user = _clients[fd];
	if (args.size() < 2) {
		send_message(fd, "Error: NICK necesita el nickname 'NICK <nickname>'");
		return;
	}
	std::string nickName = args[1];
	if (nickName.empty() || nicknameInUse(nickName)) {
		std::cout << "fd: " << fd << " error in setnickname" << std::endl;
		return;
	} else{
		user.setNickname(nickName);
		std::cout << "fd: " << fd << " setnickname '"<< nickName <<"' okey" << std::endl;
		checkRegistration(fd, user);
		return;
	} 
}
