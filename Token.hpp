#ifndef TOKEN_HPP
# define TOKEN_HPP

# include "Ircserv.hpp"

class Token
{

    public:

        enum type
        {
            KICK,
            INVITE,
            TOPIC,
            MODE 
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