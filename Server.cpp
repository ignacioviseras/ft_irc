#include "Server.hpp"
#include <fcntl.h>


Server::Server(int port, const std::string &password): _port(port), _password(password), _running(true) {
    setupServerSocket();
}

Server::~Server() {
    close(_serverSocket);
}

void Server::setupServerSocket()
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // os sockets sean no bloqueantes
    //Si un recv o un send se bloquea congelas todo el servidor para todos los usuarios
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0) {
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

    pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    _pollfds.push_back(serverPoll);
    pollfd stdinPoll;
    stdinPoll.fd = STDIN_FILENO; // FD 0
    stdinPoll.events = POLLIN;
    _pollfds.push_back(stdinPoll);

    std::cout << "Servidor iniciado en puerto " << _port << std::endl;
}

void Server::run()
{
    while (_running)
    {
        int activity = poll(&_pollfds[0], _pollfds.size(), -1);
        if (activity < 0) {
            perror("poll()");
            exit(1);
        }

        for (size_t i = 0; i < _pollfds.size(); ++i) {
            if (_pollfds[i].revents & POLLIN) {
                if (_pollfds[i].fd == _serverSocket)
                    handleNewConnection();
                else if (_pollfds[i].fd == STDIN_FILENO)
                    handleStdin();
                else
                    handleClientData(_pollfds[i].fd);
            }
        }
    }
}

void Server::handleNewConnection() {
    sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);

    int clientFd = accept(_serverSocket, (sockaddr *)&clientAddr, &addrSize);
    // os sockets sean no bloqueantes
    // Si un recv o un send se bloquea congelas todo el servidor para todos los usuarios
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl()");
        close(clientFd);
        return;
    }

    std::cout << "Cliente conectado! FD = " << clientFd << std::endl;
    _clients.insert(std::make_pair(clientFd, Client(clientFd)));

    pollfd clientPoll;
    clientPoll.fd = clientFd;
    clientPoll.events = POLLIN;
    _pollfds.push_back(clientPoll);
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
    Token token_test(token_assign_type(split(input, " ")[0]), split(input, " "));
    executeCommand(token);
    std::cout << "TOKEN GENERADO - Tipo: " << token_test.getType() << std::endl;
}

void Server::handleClientData(int fd) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        std::cout << "Cliente desconectado FD = " << fd << std::endl;
        close(fd);
        _clients.erase(fd);
        for (size_t i = 0; i < _pollfds.size(); ++i) {
            if (_pollfds[i].fd == fd) {
                _pollfds.erase(_pollfds.begin() + i);
                break;
            }
        }
        return;
    }

    buffer[bytesRead] = '\0';
    std::string msg(buffer);
    std::cout << "fd " << fd << ": '" << msg << "'";
}

void Server::executeCommand(Client *sender, const Token& token) {
    switch (token.getType()) {
        case Token::KICK:
            std::cout << "Ejecutando lógica de KICK..." << std::endl;
			_kickUser(sender, token.getArgs());
            break;
        case Token::INVITE:
            std::cout << "Ejecutando lógica de INVITE..." << std::endl;
			_inviteUser(sender, token.getArgs());
            break;
        case Token::TOPIC:
            std::cout << "Ejecutando lógica de TOPIC..." << std::endl;
			_setTopic(sender, token.getArgs());
            break;
        case Token::MODE:
            std::cout << "Ejecutando lógica de MODE..." << std::endl;
			_changeMode(sender, token.getArgs());
            break;
        default:
            std::cout << "Comando desconocido" << std::endl;
    }
}

void Server::_kickUser(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 3) {
		sendToClient(sender, "461 " + sender->getNickname() + " KICK :Not enough parameters\n");
		return;
	}

	
	std::string userToKick = args[1];
	std::cout << "Kicking user: " << userToKick << std::endl;
	// Lógica para expulsar al usuario
}

void Server::_inviteUser(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::cout << "Error: Faltan argumentos para INVITE" << std::endl;
		return;
	}
	std::string userToInvite = args[1];
	std::cout << "Inviting user: " << userToInvite << std::endl;
	// Lógica para invitar al usuario
}

void Server::_setTopic(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::cout << "Error: Faltan argumentos para TOPIC" << std::endl;
		return;
	}
	std::string newTopic = args[1];
	std::cout << "Setting new topic: " << newTopic << std::endl;
	// Lógica para establecer el tema
}

void Server::_changeMode(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		std::cout << "Error: Faltan argumentos para MODE" << std::endl;
		return;
	}
	std::string modeChange = args[1];
	std::cout << "Changing mode: " << modeChange << std::endl;
	// Lógica para cambiar el modo
}
