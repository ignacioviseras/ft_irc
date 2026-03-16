#include "../../include/Server.hpp"

void	Server::_join(int fd, const std::vector<std::string>& args) {
	if (args.size() < 2) {
        std::string errorMsg = ":irc.servidor.com 461 " + _clients[fd].getNickname() + " JOIN :Not enough parameters";
        send_message(fd, errorMsg);
        //send_message(fd, "Error: JOIN necesita un nombre de canal.");
		return;
	}

    Client& user = _clients[fd];

    // TODO: Este error es correcto? Si el canal no existe se crea, no? O solo si el nombre es inválido?
    std::string chanName = normalizeChannelName(args[1]);
    if (!isValidChannelName(chanName)) {
        std::string errorMsg = ":irc.servidor.com 403 " + user.getNickname() + " " + args[1] + " :No such channel";
        send_message(fd, errorMsg);
        return;
    }
    
    if (_channels.find(chanName) == _channels.end()) {
        _channels.insert(std::make_pair(chanName, Channel(chanName)));
        user.channels_operating.insert(chanName);
    }
    
    Channel& channel = _channels.find(chanName)->second;
    if (channel.hasUser(&user)) {
        std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
        send_message(fd, errorMsg);
        return;
    }

	//MODE I

	if (channel.getInviteMode())
	{
		//NO SE CUAL ES EL ERROR DE ESTO
		std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
        send_message(fd, errorMsg);
		return ;
	}

	// MODE L: enforce channel limit (limit < 0 means no limit)
	{
		int limit = channel.getChannelLimit();
		size_t userCount = channel.getUsers().size();
		if (limit >= 0 && userCount >= static_cast<size_t>(limit))
		{
			std::string errorMsg = ":irc.servidor.com 471 " + user.getNickname() + " " + chanName + " :Channel is full";
			send_message(fd, errorMsg);
			return;
		}
	}

	//MODE K

	if (!channel.getKey().empty() && args[3] != channel.getKey())
	{
		//NO SE CUAL ES EL ERROR DE ESTO
		std::string errorMsg = ":irc.servidor.com 443 " + user.getNickname() + " " + chanName + " :is already on channel";
        send_message(fd, errorMsg);
		return ;
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

	//sendChannelNames(&channel, serverName);
	for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
        Client* c = *it2;
        std::string namesReply = ":" + serverName + " 353 " + c->getNickname() + " = " + chanName + " :" + userList;
        std::cout << "Sending NAMES to " << c->getNickname() << ": " << namesReply << std::endl;
        send_message(c->getFd(), namesReply);
        std::string endNames = ":" + serverName + " 366 " + c->getNickname() + " " + chanName + " :End of /NAMES list.";
        std::cout << "Sending: " << endNames << std::endl;
        send_message(c->getFd(), endNames);
    }
}
