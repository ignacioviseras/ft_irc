#include "../../include/Server.hpp"

void Server::_part(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PART :Not enough parameters";
		send_message(sender->getFd(), "Error: PART necesita un nombre de canal 'PART <channel>'");
		return;
	}
	std::string chanName = normalizeChannelName(args[1]);
	if (!isValidChannelName(chanName)) {
		std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[1] + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it == _channels.end()) {
		std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + chanName + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	Channel& channel = it->second;
	if (!channel.hasUser(sender)) {
		std::string errorMsg = ":irc.servidor.com 442 " + sender->getNickname() + " " + chanName + " :You're not on that channel";
		send_message(sender->getFd(), "Error: No estás en el canal " + chanName);
		return;
	}
	std::string partMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com PART " + chanName;
	sendToChannel(channel, partMsg);
	channel.removeUser(sender);
	sender->channels_joined.erase(chanName);
	if (channel.getUsers().empty()) {
		_channels.erase(it);
	}
}
