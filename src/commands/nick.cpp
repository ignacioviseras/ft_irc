#include "../../include/Server.hpp"

bool isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 16)
		return false;
    if (std::isdigit(nick[0]) || nick[0] == '-')
		return false;
    std::string allowed = "[]\\_^{}|";
    for (size_t i = 0; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && allowed.find(nick[i]) == std::string::npos)
            return false;
    }
    return true;
}

void Server::_nick(int fd, const std::vector<std::string>& args) {
	Client& user = _clients[fd];
	if (args.size() < 2 || args[1].empty()) {
		std::string errorMsg = ":irc.servidor.com 431 " + user.getNickname() + " :No nickname given";
		send_message(fd, errorMsg);
		return;
	}
	std::string nickName = args[1];
	if (!isValidNickname(nickName)) {
        std::string errorMsg = ":irc.servidor.com 432 " + user.getNickname() + " " + nickName + " :Erroneous nickname";
        send_message(fd, errorMsg);
        return;
    }
	if (nicknameInUse(nickName)) {
		std::string errorMsg = ":irc.servidor.com 433 " + user.getNickname() + " " + nickName + " :Nickname is already in use";
		send_message(fd, errorMsg);
		return;
	} else{
		user.setNickname(nickName);
		std::cout << "fd: " << fd << " setnickname '"<< nickName <<"' okey" << std::endl;
		checkRegistration(fd, user);
		return;
	} 
}
