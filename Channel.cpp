#include "Channel.hpp"
#include "Client.hpp"
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
