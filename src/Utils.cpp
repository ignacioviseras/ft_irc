#include "../include/Utils.hpp"
#include "../include/Server.hpp"

static bool isPrintableAscii(unsigned char ch) {
    return ch >= 33 && ch <= 126;
}

static bool isLeadingCommandByte(unsigned char ch) {
    return std::isalpha(ch) || ch == ':';
}

static std::string keepAlphabeticUppercase(const std::string& rawToken) {
    std::string cleaned;

    for (std::string::const_iterator it = rawToken.begin(); it != rawToken.end(); ++it) {
        unsigned char ch = static_cast<unsigned char>(*it);

        if (std::isalpha(ch))
            cleaned += static_cast<char>(std::toupper(ch));
    }
    return cleaned;
}

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
    if (input.empty())
        return (false);
    if (input.length() > 512) { // IRC suele limitar a 512
        std::cout << "Error: Comando demasiado largo." << std::endl;
        return false;
    }
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
    if (num <= 1023 || num > 65535)
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
	else if (arg == "PART")
        return (Token::PART);
    else if (arg == "NICK")
        return (Token::NICK);
    else if (arg == "USER")
        return (Token::USER);
    else if (arg == "JOIN")
        return (Token::JOIN);
    else if (arg == "PRIVMSG")
        return (Token::PRIVMSG);
    else if (arg == "EXIT")
        return (Token::EXIT);
    else if (arg == "NAMES")
        return (Token::NAMES);
	else if (arg == "QUIT")
		return (Token::QUIT);
	else if (arg == "LIST")
		return (Token::LIST);
    else
        return (Token::UNKNOWN);
}

// UTF-8 BOM + leading spaces/tabs control
std::string sanitizeIrcLine(const std::string& rawLine)
{
    size_t start = 0;

    if (rawLine.compare(0, 3, "\xEF\xBB\xBF") == 0)
        start = 3;
    while (start < rawLine.size()) {
        unsigned char ch = static_cast<unsigned char>(rawLine[start]);

        if (isLeadingCommandByte(ch))
            break;
        if (ch == ' ' || ch == '\t' || ch < 33 || ch > 126) {
            ++start;
            continue;
        }
        break;
    }
    return rawLine.substr(start);
}

// Quita caracteres no imprimibles y espacios/tabs del principio y final de cada token
std::string sanitizeIrcToken(const std::string& rawToken)
{
    std::string cleaned;

    for (std::string::const_iterator it = rawToken.begin(); it != rawToken.end(); ++it) {
        unsigned char ch = static_cast<unsigned char>(*it);

        if (ch >= 33 && ch <= 126)
            cleaned += static_cast<char>(ch);
    }
    return cleaned;
}

// Recupera posiblemente un comando válido de un token, 
// quitando caracteres no imprimibles, espacios/tabs, 
// y buscando subcadenas que sean comandos válidos
std::string sanitizeCommandToken(const std::string& rawToken)
{
    std::string cleaned = keepAlphabeticUppercase(rawToken);

    if (cleaned.empty())
        return cleaned;
    if (Token_assign_type(cleaned) != Token::UNKNOWN)
        return cleaned;
    for (size_t i = 1; i < cleaned.size(); ++i) {
        std::string candidate = cleaned.substr(i);

        if (Token_assign_type(candidate) != Token::UNKNOWN)
            return candidate;
    }
    return cleaned;
}

std::string normalizeChannelName(const std::string& rawName)
{
    std::string cleaned;

    for (std::string::const_iterator it = rawName.begin(); it != rawName.end(); ++it) {
        unsigned char ch = static_cast<unsigned char>(*it);

        if (!isPrintableAscii(ch))
            continue;
        if (ch == ',' || ch == ':')
            continue;
        cleaned += static_cast<char>(ch);
    }

    if (cleaned.empty())
        return cleaned;
    return cleaned;
}

bool isValidChannelName(const std::string& channelName)
{
    if (channelName.length() < 2 || channelName[0] != '#')
        return false;
    for (std::string::const_iterator it = channelName.begin() + 1; it != channelName.end(); ++it) {
        unsigned char ch = static_cast<unsigned char>(*it);

        if (!isPrintableAscii(ch) || ch == ',' || ch == ':')
            return false;
    }
    return true;
}
