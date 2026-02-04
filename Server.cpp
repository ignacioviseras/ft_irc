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
	std::vector<std::string> args;
	if (!parse(input, args)) {
    	executeCommand(STDIN_FILENO, args);
	}
    //Token token_test(token_assign_type(split(input, " ")[0]), split(input, " "));
    //std::cout << "TOKEN GENERADO - Tipo: " << token_test.getType() << std::endl;
}

void Server::handleClientData(int fd) {
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    
    // leemos del sckoet
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

void Server::send_message(int fd, std::string message) {
    std::string full_message = message + "\r\n";
    if (send(fd, full_message.c_str(), full_message.length(), 0) == -1) {
        std::cerr << "Error enviando mensaje al fd: " << fd << std::endl;
    }
}
void Server::checkRegistration(int fd, Client &user) {
    if (user._isPasswordOk && !user.getUsername().empty() && !user.getNickname().empty() && !user.isRegisted()) {
        user.setRegisted(true);
        std::cout << "--- USUARIO REGISTRADO COMPLETAMENTE: " << user.getNickname() << " ---" << std::endl;
        std::string welcome = ":irc.servidor.com 001 " + user.getNickname() + " :Welcome to the IRC Network";//no se como poner los logs tanto en serv como en clinent
        send_message(fd, welcome);
    }
}

void Server::executeCommand(int fd, const std::vector<std::string>& args) {
    Token::type cmdType = token_assign_type(args[0]);

    Client& user = _clients[fd];
    if (!user._isPasswordOk && cmdType != Token::PASS) {
        send_message(fd, "Para autenticarse pruebe PASS <passwd>");
        return; 
    }
    if (user._isPasswordOk && !user.isRegisted() && 
    cmdType != Token::NICK && cmdType != Token::USER && cmdType != Token::PASS) {
        send_message(fd, "Error: Completa tu registro con NICK y USER.");
        return;
    }
    switch (cmdType) {
        //--------- KICK -----------
        case Token::KICK:
            std::cout << "Ejecutando lógica de KICK..." << std::endl;
            if (args.size() < 2) {
                send_message(fd, "Error: KICK necesita un argumento.");
                return;
            }
			_kickUser(&user, args);
            //execution kick
            break;
        //--------- INVITE -----------
        case Token::INVITE:
            std::cout << "Ejecutando lógica de INVITE..." << std::endl;
            break;
        //--------- TOPIC -----------
        case Token::TOPIC:
            std::cout << "Ejecutando lógica de TOPIC..." << std::endl;
            break;
        //--------- MODE -----------
        case Token::MODE:
            std::cout << "Ejecutando lógica de MODE..." << std::endl;
            break;
        //--------- PASS -----------
        case Token::PASS:{
            if (args.size() < 2) {
                send_message(fd, "Error: PASS necesita la contraseña.");
                return;
            }
            if (user.isRegisted())
                break;
            if (args[1] == this->_password) {
                user._isPasswordOk = true;
                send_message(fd, "password okey");
            } else 
                send_message(fd, "Error: password");
            break;
        }
        //--------- NICK -----------
        case Token::NICK: {
            if (args.size() < 2) {
                send_message(fd, "Error: NICK necesita el nickname 'NICK <nickname>'");
                return;
            }
            std::string nickName = args[1];
            if (nickName.empty() || nicknameInUse(nickName)) {
                std::cout << "fd: " << fd << " error in setnickname" << std::endl;
                return;
            } else{
                user.setNickname(nickName);
                std::cout << "fd: " << fd << " setnickname '"<< nickName <<"' okey" << std::endl;
                checkRegistration(fd, user);
                return;
            } 
            break;
        }
        //--------- USER -----------
        case Token::USER:{
            if (args.size() < 5) { // USER <username> <hostname> <servername> <realname>
                send_message(fd, "Error: USER necesita 4 argumentos <username> <hostname> <servername> <realname>");
                return;
            }
            if (user.isRegisted()) {
                send_message(fd, "Error: Ya estás registrado");
                return;
            }
            user.setUsername(args[1]);
            // hostname y servername se suelen ignorar o guardar por log
            // creo q solo tengo q guardar el args[1] -> username PREGUNTAR
            std::cout << "fd: " << fd << " Username establecido a: " << args[1] << std::endl;
            checkRegistration(fd, user);
            break;
        }
        //--------- JOIN -----------
        case Token::JOIN:
            std::cout << "Ejecutando lógica de JOIN..." << std::endl;
            break;
        //--------- PRIVMSG -----------
        case Token::PRIVMSG:
            std::cout << "Ejecutando lógica de PRIVMSG..." << std::endl;
            break;
        case Token::UNKNOWN:
        default:
            std::cerr << "Comando desconocido: " << args[0] << std::endl;
            break;
    }
}

void Server::_kickUser(Client* sender, const std::vector<std::string>& args) {
	if (args.size() < 2) {
		send_message(sender->getFd(), "Error: KICK necesita un argumento.");
		return;
	}
	std::string targetNick = args[1];
	int targetFd = -1;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getNickname() == targetNick) {
			targetFd = it->first;
			break;
		}
	}
	if (targetFd == -1) {
		send_message(sender->getFd(), "Error: Usuario no encontrado.");
		return;
	}
	send_message(targetFd, "Has sido expulsado por " + sender->getNickname());
	send_message(sender->getFd(), "Usuario " + targetNick + " expulsado correctamente.");
	close(targetFd);
	_clients.erase(targetFd);
	for (size_t i = 0; i < _pollfds.size(); ++i) {
		if (_pollfds[i].fd == targetFd) {
			_pollfds.erase(_pollfds.begin() + i);
			break;
		}
	}
}
