#include "Channel.hpp"
#include <sys/socket.h>
#include <unistd.h>

Channel::Channel(const std::string& name) : _name(name) {}

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

void Channel::sendToChannel(Channel& channel, const std::string& message, Client* exclude) {
    const std::set<Client*>& users = channel.getUsers();
    for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        Client* c = *it;
        if (exclude && c == exclude)
            continue;
        std::string full = message + "\r\n";
        send(c->getFd(), full.c_str(), full.length(), 0);
    }
}

//COMMANDS

// void    Channel::commandHub(Token tok, Client *client)
// {
//     switch (tok.getType())
//     {
//         case Token::INVITE:
//             commandInvite(client);
//             break;
//         case Token::TOPIC:
//         {
//                 if (tok.getArgs().size() <= 1 || tok.getArgs()[1].empty())
//                     commandTopic(client);
//                 else
//                     commandTopic(tok.getArgs()[1]);
//             break;
//         }
//         case Token::KICK:
//             commandKick(client);
//             break;
//         case Token::MODE:
//             commandMode(tok);
//         default:
//             break;
//     }
// }

void	Channel::commandTopic(std::string top){
    Channel::setTopic(top);
}

void	Channel::commandTopic(Client *client){
    std::string toPrint = Channel::getTopic();
    std::string full = toPrint + "\r\n";
    send(client->getFd(), full.c_str(), full.length(), 0);
}

void	Channel::commandKick(Client *client){
    Channel::removeUser(client);
}

void	Channel::commandInvite(Client *client){
    Channel::addUser(client);
}



void   Channel::commandMode(const std::vector<std::string>& args){

    if (args.size() <= 1 || args[1].empty())
        return;
    char flag = args[1][0];
    switch (flag) {
        case 'i':
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
}

void    Channel::commandModeKey(const std::vector<std::string>& args)
{

	//MUY SUJETO A CAMBIOS Y REVISION
    if (args.size() >= 3 && !args[2].empty()) {
        _key = args[2];
    }
}

void    Channel::commandModeTopic()
{
	if (_topicRestricted == true)
		_topicRestricted = false;
	else
		_topicRestricted = true;
}

void    Channel::commandModeOperator(const std::vector<std::string>& args)
{
	(void) args;
}


void    Channel::commandModeLimit(const std::vector<std::string>& args)
{

	(void)args;
	//MUY RAOR
    // if (args.size() < 3 || args[2].empty())
    //     return;
    // try {
    //     int val = std::stoi(args[2]);
    //     if (val < 0) // ignore negative limits
    //         return;
    //     _channelLimit = val;
    // } catch (const std::exception&) {
    //     // invalid number -> ignore
    // }
}
