#include "../include/Server.hpp"
#include "../include/Utils.hpp"

void create_Token()
{
    
}



// int main(int argc, char** argv) {
    
//     Server *server;
//     try
//     {
//         if (argc != 3)
//             throw Exceptions::ExceptionParams();
//         if (!valid_server_port(argv[1]))
//             throw Exceptions::NonValidPortException();
//         //faltaria comprobar la contraseña con algun tipo de formato?? ya me direis
//         server = new Server(std::atoi(argv[1]), argv[2]);
//         server.run();
//         std::string input = "";
//         std::cout << "DI AMIGO Y ENTRA (O CUALQUIER COSA) Y EXIT PARA SALIR: " << input << std::endl;
//         while (input != "EXIT")
//         {
//             std::getline(std::cin, input); 
//             std::cout << "INPUT INTRODUCIDO: " << input << std::endl;
//             if (input.empty() || !parse_commands(input))
//             {
//                 std::cout << "COMANDO INVALIDO: " << input << std::endl;
//                 continue ;
//             }
//             Token Token_test = Token(Token_assign_type(split(input, " ")[0]), split(input, " "));
//             std::cout << "Token ARGS:" << std::endl << "TYPE(enum) " << Token_test.getType() << std::endl;
//         }
//         delete(server);
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << e.what() << std::endl;
//         return 1;
//     }
//     return 0;
// }

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Error: Uso correcto: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    try {
        if (!valid_server_port(argv[1])) {
            throw Exceptions::NonValidPortException();
        }

        int port = std::atoi(argv[1]);
        std::string password = argv[2];
        
        Server server(port, password);
        std::cout << "--- Servidor IRC iniciado ---" << std::endl;
        std::cout << "Puerto: " << port << std::endl;
        std::cout << "Escribe comandos (KICK, INVITE, TOPIC, MODE) o EXIT para cerrar." << std::endl;
        server.run();

    } catch (const std::exception &e) {
        std::cerr << "Error crítico: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
