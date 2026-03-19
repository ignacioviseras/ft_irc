#include "../../include/Server.hpp"
//  :irc.example.com 324 dan #foobar +nrt

void   Server::_mode(const std::vector<std::string>& args)
{
	if (args.size() <= 2 || args[2].empty())
	{

		return;
	}
    char flag = args[2].at(0);
    switch (flag) {
        case 'i':
			std::cout << "invite" << std::endl;
            this->_modeInvite();
            break;
        case 't':
            this->_modeTopic();
            break;
        case 'k':
    		this->_modeKey(args);
            break;
        case 'o':
            this->_modeOperator(args);
            break;
        case 'l':
            this->_modeLimit(args);
            break;
        default:
            // Unknown flag
            break;

    }
}

void    Server::_modeInvite()
{
    if (_inviteOnly == true)
        _inviteOnly = false;
    else
		_inviteOnly = true;
	std::cout << "Canal restringido solo a invitaciones establecido en : " << _inviteOnly << std::endl;
}

void    Server::_modeKey(const std::vector<std::string>& args)
{

	//MUY SUJETO A CAMBIOS Y REVISION
    if (args.size() == 4 && !args[3].empty()) {
        _key = args[3];
    }
	std::cout << "Clave para unirse al canal ahora vale : " << _key << std::endl;

}

void    Server::_modeTopic()
{
	if (_topicRestricted == true)
		_topicRestricted = false;
	else
		_topicRestricted = true;
	std::cout << "Topic restringido solo a operadores establecido en : " << _topicRestricted << std::endl;
}
void    Server::_modeOperator(const std::vector<std::string>& args)
{
	
	const std::set<Client*>& users = this->getUsers();
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
	this->setOperator(c, true);
	std::cout << "Nuevo usuario asignado como operador : " << c->getUsername() << std::endl;

}

void    Server::_modeLimit(const std::vector<std::string>& args)
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

    _channelLimit = static_cast<int>(val);
	std::cout << "Limite del canal establecido en : " << _channelLimit << std::endl;
}
