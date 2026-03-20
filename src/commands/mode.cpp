#include "../../include/Server.hpp"
//  :irc.example.com 324 dan #foobar +nrt

void   Server::_mode(Client *sender, const std::vector<std::string>& args)
{

	if (args.size() == 1) {
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " MODE :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		return;
	}

    std::map<std::string, Channel>::iterator it = _channels.find(args[1]);
    if (it == _channels.end()) {
    	std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[1] + " :No such channel";
    	send_message(sender->getFd(), errorMsg);
    	return;
    }
    Channel& channel = it->second;

    if (args.size() == 2)
    {
        std::string modes = " +";
        if (channel.getInviteMode())
            modes += "i"; 
        if (channel.getTopicMode())
            modes += "t";
        if (channel.getKeyMode())
            modes += "k";
        if (channel.getLimitMode())
            modes += "l";
		if (modes.size() == 2)
			modes = "";
        std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " " + channel.getName() + modes;
        send_message(sender->getFd(), errorMsg);
        return;
    }
    if ((args[2].at(0) != '+' && args[2].at(0) != '-') || args[2].length() != 2)
    {
        std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " :Unknown MODE flag";
        send_message(sender->getFd(), errorMsg);
        return;
    }
    char flag = args[2].at(1);
    bool isAdding = args[2].at(0) == '+';
    switch (flag) {
        case 'i':
           _modeInvite(&channel, isAdding);
            break;
        case 't':
            _modeTopic(&channel, isAdding);
            break;
        case 'k':
    		_modeKey(args, &channel, isAdding);
            break;
        case 'o':
            _modeOperator(args, &channel, isAdding, sender);
            break;
        case 'l':
            _modeLimit(args, &channel, isAdding);
            break;
        default:
            std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " :Unknown MODE flag";
            send_message(sender->getFd(), errorMsg);
            break;
    }
}

void    Server::_modeInvite(Channel *channel, bool isAdding)
{
    channel->setInviteMode(isAdding);
}

void    Server::_modeKey(const std::vector<std::string>& args, Channel *channel, bool isAdding)
{
    channel->setKeyMode(isAdding);

    if (args.size() == 4 && !args[3].empty())
    {
        channel->setKey(args[3]);
    }
}

void    Server::_modeTopic(Channel *channel, bool isAdding)
{
	channel->setOperatorTopic(isAdding);
}

void    Server::_modeOperator(const std::vector<std::string>& args, Channel *channel, bool isAdding, Client *sender)
{
	if (args.size() != 4)
	{
		std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " MODE :Not enough parameters";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	const std::set<Client*>& users = channel->getUsers();
	Client* c = NULL;
	for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		c = *it;
		if (args[3] == c->getNickname())
			break ;
	}
	if (c == NULL || args[3] != c->getNickname())
	{
		std::string errorMsg = ":irc.servidor.com 441 " + args[3] + " " + channel->getName() + " :They aren't on that channel";
		send_message(sender->getFd(), errorMsg);
		return;
	}
	channel->setOperator(c, isAdding);
	std::string userList = "";
    for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
        if (!userList.empty())
			userList += " ";
		if (channel->isOperator(*it2)) userList += "@";
	}

	for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
        Client* c = *it2;
        sendNames(c->getFd(), "irc.servidor.com", c->getNickname(), channel->getName(), userList);
    }
}

void    Server::_modeLimit(const std::vector<std::string>& args, Channel *channel, bool isAdding)
{
    if (args.size() < 4 || args[3].empty())
        return;
    channel->setLimitMode(isAdding);
    const char* s = args[3].c_str();
    char* end = NULL;
    errno = 0;
    long val = std::strtol(s, &end, 10);

    if (end == s || *end != '\0' || errno == ERANGE)
        return;

    if (val < 0 || val > static_cast<long>(std::numeric_limits<int>::max()))
        return;

    channel->setLimit(static_cast<int>(val));
}
