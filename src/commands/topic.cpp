#include "../../include/Server.hpp"

void	Server::_topic(int fd, const std::vector<std::string>& args){

	std::string top;
    Client& sender = _clients[fd];

    /* std::cout << "ENTRO EN COMMANDTOPIC" << std::endl;
    std::cout << "ARG SIZE: " << args.size() << std::endl;
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << "ARG: " << i << " " << args[i] << std::endl;
    } */

    if (args.size() == 1) {
        std::string errorMsg = ":irc.servidor.com 461 " + args[0] + " :Not enough parameters";
        send_message(fd, errorMsg);
        return;
    }
    std::map<std::string, Channel>::iterator it = _channels.find(args[1]);
    if (it == _channels.end()) {
    	std::string errorMsg = ":irc.servidor.com 403 " + sender.getNickname() + " " + args[1] + " :No such channel";
    	send_message(fd, errorMsg);
    	return;
    }
    Channel& channel = it->second;

    if (args.size() == 2) {
        std::string toPrint = channel.getTopic();
        send_message(fd, toPrint);
        return;
    }
    
    if (channel.getTopicMode() && !channel.isOperator(&sender)) {
        std::string errorMsg = ":irc.servidor.com 482 " + sender.getNickname() + " " + channel.getName() + " :You're not channel operator";
        send_message(fd, errorMsg);
        return;
    }
    
    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2)
            top += " ";
        top += args[i];
    }
    if (!top.empty() && top[0] == ':')
        top.erase(0, 1);
    
    channel.setTopic(top);
}
