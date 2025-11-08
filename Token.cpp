#include "Ircserv.hpp"
#include "Token.hpp"

Token::Token()
{
}

 
Token::Token(Token::type type, std::vector<std::string> args)
{
    _type = type;
    _args = args;
}

Token::Token(const Token &other) {
    std::cout << "Copy constructor called" << std::endl;
    *this = other;
}

Token &Token::operator=(const Token &other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
    {
        _type = other._type;
        _args = other._args;
    }
    return *this;
}

Token::~Token()
{
    
}

const std::vector<std::string>& Token::getArgs() const {
    return _args;
}

// Setter for _args - takes vector<string> by const reference
void Token::setArgs(const std::vector<std::string>& args) {
    _args = args;
}

// Getter for _type - returns the enum type
Token::type Token::getType() const {
    return _type;
}

// Setter for _type
void Token::setType(type t) {
    _type = t;
}


