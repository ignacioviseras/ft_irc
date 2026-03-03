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
    std::string msg = "Adiós!!!";
    if (!reason.empty()) msg += ": " + reason;
    send_message(fd, msg);

	for (std::set<std::string>::iterator it = sender->channels_joined.begin(); it != sender->channels_joined.end(); ++it) {
		std::string chanName = *it;
		std::map<std::string, Channel>::iterator it2 = _channels.find(chanName);
		if (it2 != _channels.end()) {
			it2->second.removeUser(sender);
			std::string userList;
    		const std::set<Client*>& users = it2->second.getUsers();
    		for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
    		    if (!userList.empty()) userList += " ";
				// Añadir @ para operadores
				/* if (it2->second.isOperator(*it2))
				userList += "@"; */
				userList += (*it2)->getNickname();
			}
			std::string serverName = "irc.servidor.com";
			sendChannelNames(&it2->second, serverName);
			if (it2->second.getUsers().empty()) {
				_channels.erase(it2);
			}
		}
	    close(fd);
	    _clients.erase(fd);
	    for (size_t i = 0; i < _pollfds.size(); ++i) {
	        if (_pollfds[i].fd == fd) {
	            _pollfds.erase(_pollfds.begin() + i);
	            break;
	        }
	    }
	}
}
