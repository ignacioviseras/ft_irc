#include "../../include/Server.hpp"

void Server::_quit(Client* sender, const std::vector<std::string>& args) {	
	std::cout << "Running QUIT logic..." << std::endl;
    int fd = sender->getFd();
    std::string reason = "";
    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) reason += " ";
            reason += args[i];
        }
    }

	//delete?
	// Enviar mensaje de QUIT al cliente antes de cerrar la conexión
	// Necesario???
    std::string msg = "Bye!!!";
    if (!reason.empty()) msg += ": " + reason;
    send_message(fd, msg);

	std::set<std::string> joinedChannels(sender->channels_joined);

	for (std::set<std::string>::iterator it = joinedChannels.begin(); it != joinedChannels.end(); ++it) {
		std::string chanName = *it;
		std::map<std::string, Channel>::iterator chanIt = _channels.find(chanName);

		if (chanIt != _channels.end()) {
			Channel& channel = chanIt->second;
			chanIt->second.removeUser(sender); // Eliminar al usuario del canal

			const std::set<Client*>& remainingUsers = channel.getUsers();
			if (remainingUsers.empty()) {
				_channels.erase(chanIt); // Eliminar el canal si no quedan usuarios
				continue;
			}

			std::string quitMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com QUIT :" + reason;
			channel.sendToChannel(quitMsg, sender); // Enviar mensaje de QUIT a los demás usuarios del canal

			std::string userList = "";
            for (std::set<Client*>::const_iterator uIt = remainingUsers.begin(); uIt != remainingUsers.end(); ++uIt) {
                if (!userList.empty()) userList += " ";
                if (channel.isOperator(*uIt)) userList += "@";
                userList += (*uIt)->getNickname();
            }
			std::string serverName = "irc.servidor.com";

			for (std::set<Client*>::const_iterator uIt = remainingUsers.begin(); uIt != remainingUsers.end(); ++uIt) {
    		    Client* target = *uIt;
				Server::sendNames(target->getFd(), serverName, target->getNickname(), chanName, userList);
    		}
		}
	}

	// Cerrar la conexión del cliente y limpiar recursos
	std::cout << "Closing client connection " << sender->getNickname() << " (fd: " << fd << ")" << std::endl;
	close(fd);
	_clients.erase(fd);
}
