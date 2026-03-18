#include "../include/Channel.hpp"


Channel::Channel(const std::string& name) : _name(name), _topic("default"),_inviteOnly(false), _topicRestricted(false), _channelLimit(-1) {}

Channel::~Channel() {}

void Channel::addUser(Client* client) {
    _users.insert(client);
}


void Channel::addInvited(Client* client) {
    _invited.insert(client);
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

std::string Channel::getTopic() const {
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


const std::set<Client*>& Channel::getInvited() const {
    return _invited;
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


