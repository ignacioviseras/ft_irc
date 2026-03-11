#include "../../include/Server.hpp"

void	Server::_kick(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 3) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " KICK :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	std::string chanName = args[1];
	std::string targetNick = args[2];

	//std::cout << "HA ENTRADO EN KICK CON CANAL: " << chanName << " Y TARGET: " << targetNick << std::endl;
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it == _channels.end()) {
		std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + chanName + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		//send_message(sender->getFd(), "Error: El canal no existe.");
		return;
	}
	Channel& channel = it->second;
	if (!channel.isOperator(sender)) {
		std::string errorMsg = ":irc.servidor.com 482 " + sender->getNickname() + " " + chanName + " :You must be a channel operator";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	Client* target = findClientByNick(targetNick);
	if (!target || !channel.hasUser(target)) {
		std::string errorMsg = ":irc.servidor.com 441 " + sender->getNickname() + " " + targetNick + " " + chanName + " :They aren't on that channel";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	// Enviar KICK a todos (incluyendo al expulsado) ANTES de removerlo
	std::string kickMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com KICK " + chanName + " " + targetNick;
	sendToChannel(channel, kickMsg);
	send_message(target->getFd(), kickMsg);
	channel.removeUser(target);
	send_message(sender->getFd(), "Usuario " + targetNick + " expulsado correctamente.");
	if (channel.getUsers().empty()) {
		_channels.erase(it);
	}
}
