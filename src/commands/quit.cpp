#include "../../include/Server.hpp"

void Server::_quit(Client* sender, const std::vector<std::string>& args) {
    int fd = sender->getFd();
    std::string reason = "";
    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) reason += " ";
            reason += args[i];
        }
    }
    std::string msg = "Adiós!!!";
    if (!reason.empty()) msg += ": " + reason;
    send_message(fd, msg);

    close(fd);
    _clients.erase(fd);
    for (size_t i = 0; i < _pollfds.size(); ++i) {
        if (_pollfds[i].fd == fd) {
            _pollfds.erase(_pollfds.begin() + i);
            break;
        }
    }
}
