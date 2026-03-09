#include "../../include/Server.hpp"

void Server::_quit(Client* sender, const std::vector<std::string>& args) {	
	std::cout << "Ejecutando lógica de QUIT..." << std::endl;
    int fd = sender->getFd();
    std::string reason = "";
    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) reason += " ";
            reason += args[i];
        }
    }

	// Enviar mensaje de QUIT al cliente antes de cerrar la conexión
	// Necesario???
    std::string msg = "Adiós!!!";
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

			//sendChannelNames(&uIt->second, serverName);
			for (std::set<Client*>::const_iterator uIt = remainingUsers.begin(); uIt != remainingUsers.end(); ++uIt) {
    		    Client* target = *uIt;

				send_message(target->getFd(), quitMsg); // Enviar mensaje de QUIT a cada usuario restante

    		    std::string namesReply = ":" + serverName + " 353 " + target->getNickname() + " = " + chanName + " :" + userList;
    		    std::cout << "Sending NAMES to " << target->getNickname() << ": " << namesReply << std::endl;
    		    send_message(target->getFd(), namesReply);

    		    std::string endNames = ":" + serverName + " 366 " + target->getNickname() + " " + chanName + " :End of /NAMES list.";
    		    std::cout << "Sending: " << endNames << std::endl;
    		    send_message(target->getFd(), endNames);
    		}
		}
	}

	// Cerrar la conexión del cliente y limpiar recursos
	std::cout << "Cerrando conexión del cliente " << sender->getNickname() << " (fd: " << fd << ")" << std::endl;

	// for (size_t i = 0; i < _pollfds.size(); ++i) {
	//     if (_pollfds[i].fd == fd) {
	//         _pollfds.erase(_pollfds.begin() + i);
	//         break;
	//     }
	// }
	close(fd);
	_clients.erase(fd);
}
