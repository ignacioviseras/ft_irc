#include "../include/Exceptions.hpp"

const char *Exceptions::ExceptionParams::what() const throw() {
	return "The program need 3 params to execute ./ircserv <port> <password>";
}

const char *Exceptions::ExceptionTest::what() const throw() {
	return "test error";
}

const char *Exceptions::NonValidPortException::what() const throw() {
	return "Port value should be an integer between 1024 and 65535";
}

const char *Exceptions::NonValidCommandException::what() const throw() {
	return "Only valid commands are: KICK, INVITE, TOPIC, MODE.";
}
