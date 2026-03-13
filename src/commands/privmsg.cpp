#include "../../include/Server.hpp"

void	Server::_privMsg(Client* sender, const std::vector<std::string>& args) {
	std::cout << "Size de args en PRIVMSG: " << args.size() << std::endl;
	for (size_t i = 0; i < args.size(); ++i) {
		std::cout << "ARG: " << i << " " << args[i] << std::endl;
	}
	if (args.size() < 3) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PRIVMSG :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	std::string target = args[1];
	std::string message;
	for (size_t i = 2; i < args.size(); ++i) {
		message += args[i] + " ";
	}
	if (!message.empty())
		message.erase(message.length() - 1);

	// TODO: Esto no está repetido?
	// El código de error es correcto?
	if (!target.empty() && (target[0] == '#' || target.find('#') != std::string::npos)) {
		target = normalizeChannelName(target);
		if (!isValidChannelName(target)) {
			std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[1] + " :No such channel";
			send_message(sender->getFd(), errorMsg);
			return;
		}
		std::map<std::string, Channel>::iterator it = _channels.find(target);
		if (it == _channels.end()) {
			std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + target + " :No such channel";
			send_message(sender->getFd(), errorMsg);
			//send_message(sender->getFd(), "Error: El canal no existe.");
			return;
		}
		Channel& channel = it->second;
		if (!channel.hasUser(sender)) {
			std::string errorMsg = ":irc.servidor.com 404 " + sender->getNickname() + " " + target + " :Cannot send to channel";
			send_message(sender->getFd(), errorMsg);
			//send_message(sender->getFd(), "Error: No estás en el canal " + target);
			return;
		}
		std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com PRIVMSG " + target + " :" + message;
		channel.sendToChannel(fullMsg, sender);
	} else {
		Client* recipient = findClientByNick(target);
		if (!recipient) {
			std::string errorMsg = ":irc.servidor.com 401 " + sender->getNickname() + " " + target + " :No such nick/channel";
			send_message(sender->getFd(), errorMsg);
			//send_message(sender->getFd(), "Error: Usuario no encontrado.");
			return;
		}
		std::string fullMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com PRIVMSG " + target + " :" + message;
		send_message(recipient->getFd(), fullMsg);
	}
}
