#include "Client.hpp"

Client::Client(int fd) : _fd(fd), _registered(false) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

std::string &Client::getBuffer() {
    return _buffer;
}

bool Client::isRegisted() const {
    return _registered;
}

void Client::setRegisted(bool value) {
    _registered = value;
}
