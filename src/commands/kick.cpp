#include "../../include/Server.hpp"

void	Server::_kick(Client* sender, const std::vector<std::string>& args) {

	if (args.size() < 3) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " KICK :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		return;
	}

	const std::string chanName = normalizeChannelName(args[1]);
	if (!isValidChannelName(chanName)) {
		std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[1] + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		return ;
	}
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it == _channels.end()) {
		std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + chanName + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		return ;
	}

	Channel& channel = it->second;
	
	if (!channel.hasUser(sender)) {
		std::string errorMsg = ":irc.servidor.com 442 " + sender->getNickname() + " " + chanName + " :You're not on that channel";
		send_message(sender->getFd(), errorMsg);
		return ;
	}

	if (!channel.isOperator(sender)) {
		std::string errorMsg = ":irc.servidor.com 482 " + sender->getNickname() + " " + chanName + " :You must be a channel operator";
		send_message(sender->getFd(), errorMsg);
		return ;
	}

	// Build comment (default to sender's nickname as per spec)
    std::string comment = (args.size() >= 4) ? args[3] : sender->getNickname();

    // Parse comma-separated target users from args[2]
    std::vector<std::string> targetNicks;
    std::string targetList = args[2];
    std::string::size_type start = 0;
    std::string::size_type pos = targetList.find(',');
    while (pos != std::string::npos) {
        std::string nick = targetList.substr(start, pos - start);
        if (!nick.empty())
            targetNicks.push_back(nick);
        start = pos + 1;
        pos = targetList.find(',', start);
    }
    std::string lastNick = targetList.substr(start);
    if (!lastNick.empty())
        targetNicks.push_back(lastNick);

    for (size_t i = 0; i < targetNicks.size(); ++i) {
        const std::string& targetNick = targetNicks[i];

        Client* target = findClientByNick(targetNick);
        if (!target || !channel.hasUser(target)) {
            send_message(sender->getFd(), ":irc.servidor.com 441 " + sender->getNickname() + " " + targetNick + " " + chanName + " :They aren't on that channel");
            continue;
        }

        // Spec: one KICK message per user, with comment
        std::string kickMsg = ":" + sender->getNickname() + "!" + sender->getUsername() + "@irc.servidor.com KICK " + chanName + " " + targetNick + " :" + comment;
        sendToChannel(channel, kickMsg);

        channel.removeUser(target);

        // Re-check if channel still exists after removal
        if (channel.getUsers().empty()) {
            _channels.erase(it);
            break;
        }
    }
}