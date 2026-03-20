#include "../../include/Server.hpp"
//  :irc.example.com 324 dan #foobar +nrt

void   Server::_mode(Client *sender, const std::vector<std::string>& args)
{

    Channel* channel = findChannel(args);
	if (!channel)
	{
        std::string errorMsg = ":irc.servidor.com 403 " + sender->getNickname() + " " + args[1] + " :No such channel";
		send_message(sender->getFd(), errorMsg);
		return;
	}
    if (args.size() == 2)
    {
        std::string modes = "+";
        if (channel->getInviteMode())
            modes += "i"; 
        if (channel->getTopicMode())
            modes += "t";
        if (!channel->getKey().empty())
            modes += "k";
        if (channel->getChannelLimit() > 0)
            modes += "l";
        std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " NAMES :Not enough parameters";
        send_message(sender->getFd(), errorMsg);
        return;
    }
    if ((args[3].at(0) != '+' && args[3].at(0) != '-') || args[3].length() < 2)
    {
        std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " :Unknown MODE flag";
        send_message(sender->getFd(), errorMsg);
        return;
    }
    char flag = args[3].at(1);
    bool isAdding = args[3].at(0) == '+';
    switch (flag) {
        case 'i':
           _modeInvite(channel, isAdding);
            break;
        case 't':
            _modeTopic(channel, isAdding);
            break;
        case 'k':
    		_modeKey(args, channel, isAdding);
            break;
        case 'o':
            _modeOperator(args, channel, isAdding);
            break;
        case 'l':
            _modeLimit(args, channel, isAdding);
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
	std::cout << "Canal restringido solo a invitaciones establecido en : " << channel->getInviteMode() << std::endl;
}

void    Server::_modeKey(const std::vector<std::string>& args, Channel *channel, bool isAdding)
{
    channel->setKeyMode(isAdding);

    if (args.size() == 4 && !args[3].empty())
    {
        channel->setKey(args[3]);
    }
	std::cout << "Clave para unirse al canal ahora vale : " << channel->getKey() << std::endl;

}

void    Server::_modeTopic(Channel *channel, bool isAdding)
{
	channel->setOperatorTopic(isAdding);
	std::cout << "Topic restringido solo a operadores establecido en : " << channel->getTopicMode() << std::endl;
}

void    Server::_modeOperator(const std::vector<std::string>& args, Channel *channel, bool isAdding)
{
	
	const std::set<Client*>& users = channel-> getUsers();
	Client* c = NULL;
	for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		c = *it;
		if (args[3] == c->getUsername())
			break ;
	}
	channel->setOperator(c, isAdding);
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
	std::cout << "Limite del canal establecido en : " << channel->getChannelLimit() << std::endl;
}
