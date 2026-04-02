#include "../../include/Server.hpp"

void	Server::_invite(Client* sender, const std::vector<std::string>& args)
{
	std::cout << "1" << std::endl;
    if (args.size() < 3)
    {
        std::string line = ":irc.servidor.com 461 " + sender->getNickname() + " INVITE :Not enough parameters";
        send_message(sender->getFd(), line);
        return;
    }
	std::cout << "2" << std::endl;

    std::string targetNick = args[1];
    Client* target = findClientByNick(targetNick);
    if (!target) {
        std::string line = ":irc.servidor.com 401 " + sender->getNickname() + " " + targetNick + " :No such nick";
        send_message(sender->getFd(), line);
        return;
    }
	std::cout << "3" << std::endl;

    std::string channame = normalizeChannelName(args[2]);
    if (!isValidChannelName(channame)) {
        std::string line = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[2] + " :No such channel";
        send_message(sender->getFd(), line);
        return;
    }

	std::cout << "4" << std::endl;

    std::map<std::string, Channel>::iterator chIt = _channels.find(channame);
    if (chIt == _channels.end()) {
        std::string line = ":irc.servidor.com 403 " + sender->getNickname() + " " + channame + " :No such channel";
        send_message(sender->getFd(), line);
        return;
    }

    Channel &channel = chIt->second;
	std::cout << "5" << std::endl;

    if (!channel.hasUser(sender)) {
        std::string line = ":irc.servidor.com 442 " + sender->getNickname() + " " + channame + " :You're not on that channel";
        send_message(sender->getFd(), line);
        return;
    }
	std::cout << "6" << std::endl;

    if (channel.getInviteMode() && !channel.isOperator(sender)) {
        std::string line = ":irc.servidor.com 482 " + sender->getNickname() + " " + channame + " :You're not channel operator";
        send_message(sender->getFd(), line);
        return;
    }
	std::cout << "7" << std::endl;

    if (channel.hasUser(target)) {
        std::string line = ":irc.servidor.com 443 " + sender->getNickname() + " " + targetNick + " " + channame + " :is already on channel";
        send_message(sender->getFd(), line);
        return;
    }

	channel.addInvited(target);
	std::cout << "8" << std::endl;

    std::string reply = ":irc.servidor.com 341 " + sender->getNickname() + " " + targetNick + " " + channame;
    send_message(sender->getFd(), reply);
	std::cout << "9" << std::endl;

    std::string inviteMsg = ":" + sender->getNickname() + " INVITE " + targetNick + " :" + channame;
    send_message(target->getFd(), inviteMsg);
}
