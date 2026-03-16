#include "../include/Channel.hpp"


Channel::Channel(const std::string& name) : _name(name), _topic("default"),_inviteOnly(false), _topicRestricted(false), _channelLimit(-1) {}

Channel::~Channel() {}

void Channel::addUser(Client* client) {
    _users.insert(client);
}

void Channel::removeUser(Client* client) {
    _users.erase(client);
    _operators.erase(client);
}

bool Channel::hasUser(Client* client) const {
    return _users.count(client) > 0;
}

bool Channel::isOperator(Client* client) const {
    return _operators.count(client) > 0;
}

void Channel::setOperator(Client* client, bool op) {
    if (op) {
        _operators.insert(client);
    } else {
        _operators.erase(client);
    }
}

void    Channel::setTopic(std::string top){
    _topic = top;
}

std::string Channel::getTopic(){
    if (_topic.empty())
        return "There is no topic in the channel.";
    return _topic;
}

const std::string& Channel::getName() const {
    return _name;
}

const std::set<Client*>& Channel::getUsers() const {
    return _users;
}

//mode getters

bool Channel::getInviteMode() const {
    return _inviteOnly;
}

bool Channel::getTopicMode() const {
    return _topicRestricted;
}

int Channel::getChannelLimit() const {
	return _channelLimit;
}

std::string Channel::getKey() const {
	return _key;
}



void Channel::sendToChannel(const std::string& message, Client* exclude) {
    const std::set<Client*>& users = this->getUsers();
    for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        Client* c = *it;
        if (exclude && c == exclude)
            continue;
        std::string full = message + "\r\n";
        send(c->getFd(), full.c_str(), full.length(), 0);
    }
}

void	Channel::commandTopic(const std::vector<std::string>& args){

	std::string top;

    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2)
            top += " ";
        top += args[i];
    }
    if (!top.empty() && top[0] == ':')
        top.erase(0, 1);
	
    setTopic(top);
}

void	Channel::commandTopic(Client *client){
	(void)client;
    std::string toPrint = Channel::getTopic();
    // std::string full = toPrint + "\r\n";
	sendToChannel(toPrint, 0);
}


void   Channel::commandMode(const std::vector<std::string>& args){

	// Debug: print incoming args, vector size and each argument length
    std::cout << "commandMode args (count=" << args.size() << ", capacity=" << args.capacity() << "):";
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << " [" << i << "]=\"" << args[i] << "\"(len=" << args[i].size() << ")";
    }
    std::cout << std::endl;
    if (args.size() <= 2 || args[2].empty())
	{
		std::cout << "los primos nos fuiimos" << std::endl;
		return;

	}
	std::cout << "Flag?? :: " << args[2] << std::endl;
    char flag = args[2].at(0);
    switch (flag) {
        case 'i':
			std::cout << "invite" << std::endl;
            this->commandModeInvite();
            break;
        case 't':
            this->commandModeTopic();
            break;
        case 'k':
    		this->commandModeKey(args);
            break;
        case 'o':
            this->commandModeOperator(args);
            break;
        case 'l':
            this->commandModeLimit(args);
            break;
        default:
            // Unknown flag
            break;

    }
}

void    Channel::commandModeInvite()
{
    if (_inviteOnly == true)
        _inviteOnly = false;
    else
		_inviteOnly = true;
	std::cout << "Canal restringido solo a invitaciones establecido en : " << _inviteOnly << std::endl;
}

void    Channel::commandModeKey(const std::vector<std::string>& args)
{

	//MUY SUJETO A CAMBIOS Y REVISION
    if (args.size() == 4 && !args[3].empty()) {
        _key = args[3];
    }
	std::cout << "Clave para unirse al canal ahora vale : " << _key << std::endl;

}

void    Channel::commandModeTopic()
{
	if (_topicRestricted == true)
		_topicRestricted = false;
	else
		_topicRestricted = true;
	std::cout << "Topic restringido solo a operadores establecido en : " << _topicRestricted << std::endl;
}
void    Channel::commandModeOperator(const std::vector<std::string>& args)
{
	
	const std::set<Client*>& users = this->getUsers();
	Client* c = NULL;

	//HAY QUE COMPROBAR MUCHAS COSAS AQUI COMO QUE SUCEDE SI NO ENCUENTRA UN USER COINCIDENTE
	for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
		c = *it;
		if (args[3] == c->getUsername())
			break ;
	}
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


void    Channel::commandModeLimit(const std::vector<std::string>& args)
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
