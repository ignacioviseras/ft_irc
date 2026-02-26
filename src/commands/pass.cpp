#include "../../include/Server.hpp"

void Server::_pass(Client* sender, const std::vector<std::string>& args) {
    int fd = sender->getFd();
    if (args.size() < 2) {
        send_message(fd, "Error: PASS necesita la contraseña.");
        return;
    }
    if (sender->isRegisted())
        return send_message(fd, "Error: Ya estás registrado, no puedes usar PASS.");
    if (args[1] == this->_password) {
        sender->_isPasswordOk = true;
        send_message(fd, "password okey");
    } else 
        send_message(fd, "Error: password");
}