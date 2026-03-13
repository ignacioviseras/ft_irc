#include "../include/Server.hpp"
#include <fcntl.h>


Server::Server(int port, const std::string &password): _port(port), _password(password), _running(true) {
    setupServerSocket();
}

Server::~Server() {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        close(it->first);
    }
    _clients.clear();
    if (_epollFd != -1)
        close(_epollFd);
    if (_serverSocket != -1)
        close(_serverSocket);
}

void Server::setupServerSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0) {
        perror("socket()");
        exit(1);
    }
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0) {//socket no bloqueante
        perror("fcntl()");
        exit(1);
    }

    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(_port);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    memset(servAddr.sin_zero, 0, sizeof(servAddr.sin_zero));
    if (bind(_serverSocket, (sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind()");
        exit(1);
    }
    if (listen(_serverSocket, SOMAXCONN) < 0) {
        perror("listen()");
        exit(1);
    }

    _epollFd = epoll_create1(0);
    if (_epollFd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    // anadimos _serverSocket al epoll
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = _serverSocket;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverSocket, &ev) == -1) {
        perror("epoll_ctl: serverSocket");
        exit(1);
    }

    // añadimos STDIN al epoll
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl: stdin");
    }

    std::cout << "Servidor IRC con epoll iniciado en puerto " << _port << std::endl;
}

void Server::run() {
    struct epoll_event events[64]; // donde epoll_wait dejará los resultados

    while (_running) {
        int nfds = epoll_wait(_epollFd, events, 64, -1);
        if (nfds < 0)
            break;

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            // manejo de errores o desconexiones
            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                std::cout << "Error/HUP en FD " << fd << ": Desconectando......." << std::endl;
                disconnectClient(fd);
                continue;
            }

            // manejo de lectura
            if (events[i].events & EPOLLIN) {
                if (fd == _serverSocket)
                    handleNewConnection();
                else if (fd == STDIN_FILENO)
                    handleStdin();
                else
                    handleClientData(fd);
            }

            // manejo de escritura     solo si hay algo en el buffer del cliente
            if (events[i].events & EPOLLOUT) {
                handleClientWrite(fd);
            }
        }
    }
}

void Server::handleNewConnection() {
    sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);

    int clientFd = accept(_serverSocket, (sockaddr *)&clientAddr, &addrSize);
    // Si un recv o un send se bloquea congelas todo el servidor para todos los usuarios
    if (clientFd < 0) {
        perror("accept()");
        return;
    }
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl()");
        close(clientFd);
        return;
    }

    std::cout << "Nuevo cliente: FD " << clientFd << std::endl;
    _clients.insert(std::make_pair(clientFd, Client(clientFd)));
    struct epoll_event ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN; // Solo lectura al principio
    ev.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        perror("epoll_ctl: add client");
        close(clientFd);
        _clients.erase(clientFd);
    }
}

void Server::handleStdin() {
    std::string input;
    if (!std::getline(std::cin, input))
        return;
    if (input == "EXIT") {
        _running = false;
        std::cout << "Cerrando servidor..." << std::endl;
    }
    if (input.empty())
        return;
    if (!parse_commands(input)) {
        return;
    }
	std::vector<std::string> args;
	if (!parse(input, args)) {
    	executeCommand(STDIN_FILENO, args);
	}
    //Token Token_test(Token_assign_type(split(input, " ")[0]), split(input, " "));
    //std::cout << "Token GENERADO - Tipo: " << Token_test.getType() << std::endl;
}

void Server::handleClientData(int fd) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    
    // leemos del sckoet
    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        std::cout << "Cliente desconectado FD = " << fd << std::endl;
        disconnectClient(fd);
        return;
    }
    _clients[fd].getBuffer().append(buffer, bytesRead);// acumulamos los datos

    // tramitamos el buffer -> \n
    std::string &clientBuff = _clients[fd].getBuffer();
    size_t pos = 0;
    while ((pos = clientBuff.find('\n')) != std::string::npos) {
        std::string commandLine = clientBuff.substr(0, pos);// extraemos el comando (como un gnl)
        if (!commandLine.empty() && commandLine[commandLine.length() - 1] == '\r')// irc usa \r\n quitamos los \r
            commandLine.erase(commandLine.length() - 1);

        if (!commandLine.empty()) {
            std::cout << "fd: " << fd << ": " << commandLine << std::endl;
            handleCommand(fd, commandLine);
        }
        clientBuff.erase(0, pos + 1);//borramos lo procesado para no tener un bucle q leemos lo mismo (como en el gnl)
    }
    // Lo que sobre (si no hay \n) se queda en clientBuff esperando al siguiente
}

bool Server::parse(const std::string& commandLine, std::vector<std::string>& args) {
    if (!parse_commands(commandLine))
        return false;
    std::vector<std::string> rawArgs = split(commandLine, " ");
    for (size_t i = 0; i < rawArgs.size(); ++i) {
        if (!rawArgs[i].empty())
            args.push_back(rawArgs[i]);
    }
    if (args.empty())
        return false;

    return true;
}

void Server::handleCommand(int fd, std::string& commandLine) {
    std::vector<std::string> args;
    if (!parse(commandLine, args))
        return;
    executeCommand(fd, args);
}

bool Server::nicknameInUse(const std::string& nick) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nick) {
            return true;
        }
    }
    return false;
}

void Server::handleClientWrite(int fd) {
    if (_clients.find(fd) == _clients.end())
        return;

    Client &user = _clients[fd];
    std::string &buffer = user.getWriteBuffer();
    if (buffer.empty()) {// si el buffer esta vacio quitamos EPOLLOUT
        struct epoll_event ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
        return;
    }

    ssize_t bytesSent = send(fd, buffer.c_str(), buffer.length(), 0);
    if (bytesSent > 0)
        buffer.erase(0, bytesSent); // quiraqmos del buffer lo que se envió
    else if (bytesSent < 0) {// Si da 0 o negativo, cerramos la conexión SIN mirar errno
        perror("send()");
        disconnectClient(fd);
        return;
    }
    

    // quitamos EPOLLOUT solo cuando el buffer este vacio
    if (buffer.empty()) {
        struct epoll_event ev;
        std::memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;  // cambiuanos EPOLLOUT a EPOLLIN
        ev.data.fd = fd;
        epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev);
    }
}

void Server::send_message(int fd, std::string message) {
    if (_clients.find(fd) == _clients.end())
        return;

    Client &user = _clients[fd];
    struct epoll_event ev;

    user.getWriteBuffer().append(message + "\r\n");
    std::memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLOUT; //dmoificamos el registro para decirle a epoll que queremos escribir (EPOLLOUT)
    ev.data.fd = fd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        perror("epoll_ctl: mod EPOLLOUT");
    }
}

void Server::checkRegistration(int fd, Client &user) {
    if (user.isRegisted()) {
        if (!user._isRegistered) {
            user.setRegisted(true);
            std::string welcome = ":irc.servidor.com 001 " + user.getNickname() + " :Welcome to the IRC Network"; // no se como poner los logs tanto en serv como en client
            send_message(fd, welcome);
        }
    }
}

//mixi

Channel* Server::findServer(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
		return NULL;
	}
    std::string chanName = normalizeChannelName(args[1]);
    if (!isValidChannelName(chanName)) {
        return NULL;
    }
	std::map<std::string, Channel>::iterator it = _channels.find(chanName);
	if (it == _channels.end()) {
		_channels.insert(std::make_pair(chanName, Channel(chanName)));
		it = _channels.find(chanName);
	}
	Channel& channel = it->second;
	return (&channel);
}

void Server::executeCommand(int fd, const std::vector<std::string>& args) {
    Token::type cmdType = Token_assign_type(args[0]);

	int nbr = 0;
	for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it) {
		std::cout << "ARG: " << nbr << " " << *it << std::endl;
		nbr++;
	}
    Client& user = _clients[fd];
    if (!user._isPasswordOk && cmdType != Token::PASS) {
        send_message(fd, ":irc.servidor.com 451 * :You have not registered PASS <passwd>.");
        return; 
    }
    if (cmdType == Token::PASS && user._isPasswordOk) {
        send_message(fd, ":irc.servidor.com 462 " + user.getNickname() + " :You may not reregister");
        return;
    }
    if (cmdType == Token::USER && !user.getUsername().empty()) {
        send_message(fd, ":irc.servidor.com 462 " + user.getNickname() + " :You may not reregister");
        return;
    }
    if (user._isPasswordOk && !user.isRegisted() && 
    cmdType != Token::NICK && cmdType != Token::USER && cmdType != Token::PASS) {
        send_message(fd, ":irc.servidor.com 451 * :You have not registered NICK y USER.");
        return;
    }
	Channel *c = findServer(args);
    switch (cmdType) {
        //--------- USER -----------
        case Token::USER:{
            _user(&user, args);
            break;
        }
        //--------- NICK -----------
        case Token::NICK: {
            _nick(fd, args);
            break;
        }
        //--------- PASS -----------
        case Token::PASS:{
			_pass(&user, args);
            break;
        }
		case Token::PART: {
			_part(&user, args);
			break;
		}
        //--------- JOIN -----------
        case Token::JOIN: {
            _join(fd, args);
            break;
        }
        //--------- KICK -----------
        case Token::KICK:
		{ 
			_kick(&user, args);
            break;
		}
        //--------- INVITE -----------
        case Token::INVITE:
            //c->commandInvite(&user, args);
            break;
        //--------- TOPIC -----------
        case Token::TOPIC:
		{
                if (args.size() <= 2)
                    c->commandTopic(&user);
                else
                    c->commandTopic(args);
            break;
        }
        //--------- MODE -----------
        case Token::MODE:
        	c->commandMode(args);
            break;
		case Token::QUIT: {
            _quit(&user, args);
			break;
		}
        case Token::NAMES: {
            _names(fd, args);
            break;
        }
        //--------- PRIVMSG -----------
        case Token::PRIVMSG:
            std::cout << "Ejecutando lógica de PRIVMSG..." << std::endl;
			_privMsg(&user, args);
            break;
        case Token::UNKNOWN:
			//sea lo que sea la gestion que hay que hacer aqui.
			break;
        default:
            std::cerr << "Comando desconocido: " << args[0] << std::endl;
            break;
    }
}

Client* Server::findClientByNick(const std::string& nick) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nick) {
            return &it->second;
        }
    }
    return NULL;
}

void Server::sendToChannel(const Channel& channel, const std::string& msg) {
    const std::set<Client*>& users = channel.getUsers();
    for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        send_message((*it)->getFd(), msg);
    }
}

void Server::disconnectClient(int fd) {
    std::map<int, Client>::iterator clientIt = _clients.find(fd);
    if (clientIt == _clients.end())
        return;
    Client* client = &clientIt->second;
    std::vector<std::string> channelsToRemove;
    for (std::map<std::string, Channel>::iterator chanIt = _channels.begin(); 
         chanIt != _channels.end(); ++chanIt) {
        if (chanIt->second.hasUser(client)) {
            chanIt->second.removeUser(client);
            // Mark empty channels for removal
            if (chanIt->second.getUsers().empty()) {
                channelsToRemove.push_back(chanIt->first);
            }
        }
    }
    for (std::vector<std::string>::iterator it = channelsToRemove.begin(); 
         it != channelsToRemove.end(); ++it) {
        _channels.erase(*it);
    }
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
}
