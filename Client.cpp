#include "Client.hpp"

Client::Client() : _isRegistered(false), _isPasswordOk(false), _fd(-1) {}
Client::Client(int fd) :  _isRegistered(false), _isPasswordOk(false), _fd(fd) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

std::string &Client::getBuffer() {
    return _buffer;
}

bool Client::isRegisted() const {
    return _isPasswordOk && !_nickname.empty() && !_username.empty();
}

void Client::setRegisted(bool value) {
    _isRegistered = value;
}

void Client::setUsername(const std::string &username) {
    _username = username;
}

void Client::setNickname(const std::string &nickname) {
    _nickname = nickname;
}

std::string &Client::getNickname() {
    return _nickname;
}

std::string &Client::getUsername() {
    return _username;
}
