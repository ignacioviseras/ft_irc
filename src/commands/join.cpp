#include "../../include/Server.hpp"

void	Server::_join(int fd, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		send_message(fd, "Error: JOIN necesita un nombre de canal.");
		return;
	}
    Client& user = _clients[fd];
    std::string chanName = args[1];
    
    // Añadir # al nombre del canal si no lo tiene
    if (chanName[0] != '#') {
        chanName = "#" + chanName;
    }
    
    std::map<std::string, Channel>::iterator it = _channels.find(chanName);
    if (it == _channels.end()) {
		_channels.insert(std::make_pair(chanName, Channel(chanName)));
        it = _channels.find(chanName);
		user.channels_operating.insert(chanName);
	}
    Channel& channel = it->second;
    if (channel.hasUser(&user)) {
        send_message(fd, "Error: Ya estás en el canal " + chanName);
        return;
    }
    channel.addUser(&user);
    if (channel.getUsers().size() == 1) {
        channel.setOperator(&user, true);
    }
    std::string serverName = "irc.servidor.com";
    // Enviar mensaje de JOIN a todos los usuarios del canal
    std::string prefix = ":" + user.getNickname() + "!" + user.getUsername() + "@" + serverName;
    std::string joinMsg = prefix + " JOIN :" + chanName;
    sendToChannel(channel, joinMsg);
    // Construir la lista de usuarios para el mensaje NAMES
    std::string userList;
    const std::set<Client*>& users = channel.getUsers();
    for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
        if (!userList.empty()) userList += " ";
		// Añadir @ para operadores
		if (channel.isOperator(*it2))
		userList += "@";
		userList += (*it2)->getNickname();
	}
    
	
    // Enviar NAMES a todos los usuarios del canal para mostrar la lista actualizada
    for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
        Client* c = *it2;
        std::string namesReply = ":" + serverName + " 353 " + c->getNickname() + " = " + chanName + " :" + userList;
        std::cout << "Sending NAMES to " << c->getNickname() << ": " << namesReply << std::endl;
        send_message(c->getFd(), namesReply);
        std::string endNames = ":" + serverName + " 366 " + c->getNickname() + " " + chanName + " :End of /NAMES list.";
        std::cout << "Sending: " << endNames << std::endl;
        send_message(c->getFd(), endNames);
    }
	user.channels_joined.insert(chanName);
}
