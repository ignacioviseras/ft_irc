#include "../../include/Server.hpp"

void Server::sendNames(int fd, std::string serverName, std::string nickname, std::string chanName, std::string userList) {
	// Send NAMES reply
	std::string namesReply = ":" + serverName + " 353 " + nickname + " = " + chanName + " :" + userList;
	send_message(fd, namesReply);

	// Send end of NAMES
	std::string endNames = ":" + serverName + " 366 " + nickname + " " + chanName + " :End of /NAMES list.";
	send_message(fd, endNames);
}

void Server::_names(int fd, const std::vector<std::string>& args) {
	Client& user = _clients[fd];
	std::string serverName = "irc.servidor.com";

	if (args.size() < 2) {
		std::string errorMsg = ":irc.servidor.com 461 " + user.getNickname() + " NAMES :Not enough parameters";
		send_message(fd, errorMsg);
		return;
	}

	std::vector<std::string> rawChannels = split(args[1], ",");
	for (size_t i = 0; i < rawChannels.size(); ++i) {
		std::string chanName = normalizeChannelName(rawChannels[i]);
		std::map<std::string, Channel>::iterator it = _channels.find(chanName);
		if (it == _channels.end()) {
			std::string errorMsg = ":irc.servidor.com 403 " + user.getNickname() + " " + chanName + " :No such channel";
			send_message(fd, errorMsg);
			continue;
		}
		Channel& channel = it->second;

		// Build user list with operator prefixes
		std::string userList = "";
		const std::set<Client*>& users = channel.getUsers();
		for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
			if (!userList.empty())
				userList += " ";
			if (channel.isOperator(*it)) userList += "@"; // Add @ for operators
			userList += (*it)->getNickname();
		}
		sendNames(fd, serverName, user.getNickname(), chanName, userList);
		}
}
