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
    if (args.size() == 2) {

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
    char flag = args[2].at(0);
    switch (flag) {
        case 'i':
			std::cout << "invite" << std::endl;
           _modeInvite(channel);
            break;
        case 't':
            _modeTopic(channel);
            break;
        case 'k':
    		_modeKey(args, channel);
            break;
        case 'o':
            _modeOperator(args, channel);
            break;
        case 'l':
            _modeLimit(args, channel);
            break;
        default:
            // Unknown flag
            break;
    }
}

void    Server::_modeInvite(Channel *channel)
{
    if (channel->getInviteMode() == true)
        channel->setInviteMode(false);
    else
		channel->setInviteMode(true);
	std::cout << "Canal restringido solo a invitaciones establecido en : " << channel->getInviteMode() << std::endl;
}

void    Server::_modeKey(const std::vector<std::string>& args, Channel *channel)
{

	//MUY SUJETO A CAMBIOS Y REVISION
    if (args.size() == 4 && !args[3].empty()) {
        channel->setKey(args[3]);
    }
	std::cout << "Clave para unirse al canal ahora vale : " << channel->getKey() << std::endl;

}

void    Server::_modeTopic(Channel *channel)
{
	if (channel->getTopicMode() == true)
		channel->setOperatorTopic(false);
	else
		channel->setOperatorTopic(true);
	std::cout << "Topic restringido solo a operadores establecido en : " << channel->getTopicMode() << std::endl;
}

void    Server::_modeOperator(const std::vector<std::string>& args, Channel *channel)
{
	
	const std::set<Client*>& users = channel-> getUsers();
	Client* c = NULL;

	//HAY QUE COMPROBAR MUCHAS COSAS AQUI COMO QUE SUCEDE SI NO ENCUENTRA UN USER COINCIDENTE
	for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		c = *it;
		if (args[3] == c->getUsername())
			break ;
	}
    //delete?
	// std::string userList = "";
    // for (std::set<Client*>::const_iterator it2 = users.begin(); it2 != users.end(); ++it2) {
    //     if (!userList.empty())
	// 		userList += " ";
	// 	if (this->isOperator(*it2)) userList += "@"; // Añadir @ para operadores
	// 	userList += (*it2)->getNickname();
	// }
	channel->setOperator(c, true);
	std::cout << "Nuevo usuario asignado como operador : " << c->getUsername() << std::endl;

}

void    Server::_modeLimit(const std::vector<std::string>& args, Channel *channel)
{
    if (args.size() < 4 || args[3].empty())
        return;

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
