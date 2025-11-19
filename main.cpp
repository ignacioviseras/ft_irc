#include "Ircserv.hpp"

//split guarro joseado para salir del paso
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end, delim_len = delimiter.length();
    while ((end = s.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delim_len;
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

void create_token()
{
    
}

bool parse_commands(std::string input)
{
    std::vector<std::string> splitted_input = split(input, " ");
    // for (size_t i = 0; i < splitted_input.size(); ++i)
    //     std::cout << "PARTE " << i << " : " << splitted_input[i] << std::endl;
    if (splitted_input[0] != "KICK" && splitted_input[0] != "INVITE" && splitted_input[0] != "TOPIC" && splitted_input[0] != "MODE")
    {
        std::cout << splitted_input[0] << " no es un comando. Prueba con KICK, INVITE, TOPIC o MODE" << std::endl;
        return false;
    }
    std::cout << splitted_input[0] << "fue valido" << std::endl;
    int token_size = splitted_input.size();
    //esta comprobacion es 
    if (token_size > 4)
        return (false);
    return (true);
}

bool valid_server_port(char *arg)
{
    unsigned int num;
    for (size_t i = 0; i < std::strlen(arg); i++)
    {
        if (!isdigit(arg[i]))
            return (false);
    }
    num = std::atoi(arg);
    if (num > 65535)
        return (false);
    return (true);
}
//Asdasd

Token::type token_assign_type(std::string arg)
{
    if (arg == "KICK")
        return (Token::KICK);
    else if (arg == "INVITE")
        return (Token::INVITE);
    else if (arg == "TOPIC")
        return (Token::TOPIC);
    else
        return (Token::MODE);
}

int main(int argc, char** argv) {
    
    Ircserv *server;
    try
    {
        if (argc != 3)
            throw Exceptions::ExceptionParams();
        if (!valid_server_port(argv[1]))
            throw Exceptions::NonValidPortException();
        //faltaria comprobar la contraseña con algun tipo de formato?? ya me direis
        server = new Ircserv(std::atoi(argv[1]), argv[2]);
        std::string input = "";
        std::cout << "DI AMIGO Y ENTRA (O CUALQUIER COSA) Y EXIT PARA SALIR: " << input << std::endl;
        while (input != "EXIT")
        {
            std::getline(std::cin, input); 
            std::cout << "INPUT INTRODUCIDO: " << input << std::endl;
            if (input.empty() || !parse_commands(input))
            {
                std::cout << "COMANDO INVALIDO: " << input << std::endl;
                continue ;
            }
            Token token_test = Token(token_assign_type(split(input, " ")[0]), split(input, " "));
            std::cout << "TOKEN ARGS:" << std::endl << "TYPE(enum) " << token_test.getType() << std::endl;
        }
        delete(server);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}