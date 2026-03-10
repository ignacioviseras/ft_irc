#include "../../include/Server.hpp"

void Server::_pass(Client* sender, const std::vector<std::string>& args) {
    int fd = sender->getFd();
    if (args.size() < 2) {
        std::string errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PASS :Not enough parameters";
        send_message(fd, errorMsg);
        return;
    }
    if (sender->isRegisted()) {
        std::string errorMsg = ":irc.servidor.com 462 " + sender->getNickname() + " :Unauthorized command (already registered)";
        return send_message(fd, errorMsg);
    }
    if (args[1] == this->_password) {
        sender->_isPasswordOk = true;
        send_message(fd, "password okey");
    } else {
        std::string errorMsg = ":irc.servidor.com 464 " + sender->getNickname() + " :Password incorrect";
        send_message(fd, errorMsg);
    }
}