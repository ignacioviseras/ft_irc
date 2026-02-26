#include "../include/Utils.hpp"

//split guarro joseado para salir del paso
std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> Tokens;
    size_t start = 0, end, delim_len = delimiter.length();
    while ((end = s.find(delimiter, start)) != std::string::npos) {
        Tokens.push_back(s.substr(start, end - start));
        start = end + delim_len;
    }
    Tokens.push_back(s.substr(start));
    return Tokens;
}

bool parse_commands(const std::string& input)
{
    // std::vector<std::string> splitted_input = split(input, " ");
    // for (size_t i = 0; i < splitted_input.size(); ++i)
    //     std::cout << "PARTE " << i << " : " << splitted_input[i] << std::endl;
    // if (splitted_input[0] != "KICK" && splitted_input[0] != "INVITE" && splitted_input[0] != "TOPIC" && splitted_input[0] != "MODE")
    // {
    //     std::cout << splitted_input[0] << " no es un comando. Prueba con KICK, INVITE, TOPIC o MODE" << std::endl;
    //     return false;
    // }
    // std::cout << splitted_input[0] << " fue valido" << std::endl;
    if (input.empty())
        return (false);
    if (input.length() > 512) { // IRC suele limitar a 512
        std::cout << "Error: Comando demasiado largo." << std::endl;
        return false;
    }
    // int Token_size = splitted_input.size();
    // //esta comprobacion es 
    // if (Token_size > 4)
    //     return (false);
    return (true);
}

bool valid_server_port(const char *arg)
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

Token::type Token_assign_type(const std::string& arg)
{
    if (arg == "KICK")
        return (Token::KICK);
    else if (arg == "INVITE")
        return (Token::INVITE);
    else if (arg == "TOPIC")
        return (Token::TOPIC);
    else if (arg == "MODE")
        return (Token::MODE);
    else if (arg == "PASS")
        return (Token::PASS);
    else if (arg == "NICK")
        return (Token::NICK);
    else if (arg == "USER")
        return (Token::USER);
    else if (arg == "JOIN")
        return (Token::JOIN);
    else if (arg == "PRIVMSG")
        return (Token::PRIVMSG);
	else if (arg == "QUIT")
		return (Token::QUIT);
    else
        return (Token::UNKNOWN);
}