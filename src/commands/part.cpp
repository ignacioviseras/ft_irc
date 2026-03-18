#include "../../include/Server.hpp"

void Server::_part(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PART :Not enough parameters";
		send_message(sender->getFd(), "Error: PART necesita un nombre de canal 'PART <channel>'");
		return;
	}

	std::vector<std::string> rawChannels = split(args[1], ",");
	for (size_t i = 0; i < rawChannels.size(); ++i) {

		std::string chanName = normalizeChannelName(rawChannels[i]);

		std::map<std::string, Channel>::iterator it = _channels.find(chanName);
		if (it == _channels.end()) {
			std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + chanName + " :No such channel";
			send_message(sender->getFd(), errorMsg);
			continue;
		}
		Channel& channel = it->second;
		if (!channel.hasUser(sender)) {
			std::string errorMsg = ":irc.servidor.com 442 " + sender->getNickname() + " " + chanName + " :You're not on that channel";
			send_message(sender->getFd(), errorMsg);
			continue;
		}
		std::string partMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com PART " + chanName;
		sendToChannel(channel, partMsg);
		channel.removeUser(sender);
		sender->channels_joined.erase(chanName);
		if (channel.getUsers().empty()) {
			_channels.erase(it);
		}
	}
}
