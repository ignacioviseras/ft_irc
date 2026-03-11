#include "../../include/Server.hpp"

void Server::_names(int fd, const std::vector<std::string>& args) {
	Client& user = _clients[fd];
	std::string serverName = "irc.servidor.com";

	if (args.size() < 2) {
		std::string errorMsg = ":irc.servidor.com 461 " + user.getNickname() + " NAMES :Not enough parameters";
		send_message(fd, errorMsg);
		return;
	}

	std::string chanName = args[1];
	// Add # to channel name if not present
	if (chanName[0] != '#') chanName = "#" + chanName;

	// Check if channel exists
	if (_channels.find(chanName) == _channels.end()) {
		std::string endNames = ":" + serverName + " 366 " + user.getNickname() + " " + chanName + " :End of /NAMES list.";
		send_message(fd, endNames);
		return;
	}

	Channel& channel = _channels.find(chanName)->second;

	// Build user list with operator prefixes
	std::string userList = "";
	const std::set<Client*>& users = channel.getUsers();
	for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		if (!userList.empty())
			userList += " ";
		if (channel.isOperator(*it)) userList += "@"; // Add @ for operators
		userList += (*it)->getNickname();
	}

	// Send NAMES reply
	std::string namesReply = ":" + serverName + " 353 " + user.getNickname() + " = " + chanName + " :" + userList;
	send_message(fd, namesReply);

	// Send end of NAMES
	std::string endNames = ":" + serverName + " 366 " + user.getNickname() + " " + chanName + " :End of /NAMES list.";
	send_message(fd, endNames);
}
