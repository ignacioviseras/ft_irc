#include "../../include/Server.hpp"

void	Server::_join(int fd, const std::vector<std::string>& args) {
	if (args.size() < 2) {
        std::string errorMsg = ":irc.servidor.com 461 " + _clients[fd].getNickname() + " JOIN :Not enough parameters";
        send_message(fd, errorMsg);
		return;
	}

    Client& user = _clients[fd];

	std::vector<std::string> rawChannels = split(args[1], ",");
	std::vector<std::string> keys;
	if (args.size() >= 3)
		keys = split(args[2], ",");

	for (size_t i = 0; i < rawChannels.size(); ++i) {
		std::string chanName = normalizeChannelName(rawChannels[i]);

		if (!isValidChannelName(chanName)) {
			std::string badName = rawChannels[i].empty() ? args[1] : rawChannels[i];
			std::string errorMsg = ":irc.servidor.com 403 " + user.getNickname() + " " + badName + " :No such channel";
			send_message(fd, errorMsg);
			continue;
		}

		if (_channels.find(chanName) == _channels.end()) {
			_channels.insert(std::make_pair(chanName, Channel(chanName)));
			user.channels_operating.insert(chanName);
		}

		Channel& channel = _channels.find(chanName)->second;
		if (channel.hasUser(&user)) {
			std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
			send_message(fd, errorMsg);
			continue;
		}

		//MODE I

		if (channel.getInviteMode())
		{
			//NO SE CUAL ES EL ERROR DE ESTO
			std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
    	    send_message(fd, errorMsg);
			continue;
		}

		// MODE L: enforce channel limit (limit < 0 means no limit)
		int limit = channel.getChannelLimit();
		size_t userCount = channel.getUsers().size();
		if (limit >= 0 && userCount >= static_cast<size_t>(limit))
		{
			std::string errorMsg = ":irc.servidor.com 471 " + user.getNickname() + " " + chanName + " :Channel is full";
			send_message(fd, errorMsg);
			continue;
		}

		//MODE K
		std::string providedKey;
		if (i < keys.size())
			providedKey = keys[i];

		if (!channel.getKey().empty() && providedKey != channel.getKey())
		{
			//NO SE CUAL ES EL ERROR DE ESTO
			std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
    	    send_message(fd, errorMsg);
			continue;
		}

		channel.addUser(&user);
		user.channels_joined.insert(chanName);
    	if (channel.getUsers().size() == 1) {
    	    channel.setOperator(&user, true);
    	}

		std::string serverName = "irc.servidor.com";
    	// Enviar mensaje de JOIN a todos los usuarios del canal
    	std::string prefix = ":" + user.getNickname() + "!" + user.getUsername() + "@" + serverName;
    	std::string joinMsg = prefix + " JOIN :" + chanName;
    	sendToChannel(channel, joinMsg);

    	// Construir la lista de usuarios para el mensaje NAMES
    	std::string userList = "";
    	const std::set<Client*>& users = channel.getUsers();
    	for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
    	    if (!userList.empty())
				userList += " ";
			if (channel.isOperator(*it2)) userList += "@"; // Añadir @ para operadores
			userList += (*it2)->getNickname();
		}

		for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
    	    Client* c = *it2;
    	    Server::sendNames(c->getFd(), serverName, c->getNickname(), chanName, userList);
    	}
	}
}
