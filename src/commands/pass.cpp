#include "../../include/Server.hpp"

void Server::_pass(Client* sender, const std::vector<std::string>& args) {
    int fd = sender->getFd();

	if (sender->isRegisted()) {
        std::string errorMsg = ":irc.servidor.com 462 " + sender->getNickname() + " :Unauthorized command (already registered)";
        return send_message(fd, errorMsg);
    }
    if (args.size() != 2) {
        std::string errorMsg;
        if (args.size() < 2) {
            errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PASS :Not enough parameters";
        } else {
            errorMsg = ":irc.servidor.com 461 " + sender->getNickname() + " PASS :Too many parameters";
        }
        send_message(fd, errorMsg);
        return;
    }
    if (args[1] == this->_password) {
        sender->_isPasswordOk = true;
        send_message(fd, "password okey");
    } else {
        std::string errorMsg = ":irc.servidor.com 464 " + sender->getNickname() + " :Password incorrect";
        send_message(fd, errorMsg);
    }
}