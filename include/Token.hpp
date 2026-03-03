#ifndef TOKEN_HPP
# define TOKEN_HPP

# include <vector>
# include <string>
# include <iostream>


class Token
{

    public:

        enum type {
            USER, //auth
            NICK, //auth
            PASS, //auth
			PART, //para salir de un canal
			QUIT, //para desconectar
            JOIN,
            KICK,
            INVITE,
            TOPIC,
            MODE,
            PRIVMSG, // para mensajes
            UNKNOWN //se puede usar por si no existe y asi se tramita

        };

        Token();
        Token(Token::type type, std::vector<std::string> args);
        Token(const Token &copy);
        Token &operator=(const Token &other);
        ~Token();
        
        const std::vector<std::string>& getArgs() const;
        void setArgs(const std::vector<std::string>& args);
        type getType() const;
        void setType(type t);


    private:
        std::vector<std::string> _args;
        type _type;

};

#endif