#include "../include/Server.hpp"
#include "../include/Utils.hpp"

//delete?
void create_Token()
{
    
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Error: Correct use: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    try {
        if (!valid_server_port(argv[1]))
            throw Exceptions::NonValidPortException();
        int port = std::atoi(argv[1]);
        std::string password = argv[2];
        
        Server server(port, password);
        std::cout << "--- IRC server started ---" << std::endl;
        std::cout << "Port: " << port << std::endl;
        std::cout << "Write commands (KICK, INVITE, TOPIC, MODE) o EXIT to close." << std::endl;
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
