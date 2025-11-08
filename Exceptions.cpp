#include "Exceptions.hpp"

const char *Exceptions::ExceptionParams::what() const throw() {
	return "The program need 3 params to execute";
}

const char *Exceptions::ExceptionTest::what() const throw() {
	return "que es lo que pasa";
}

const char *Exceptions::NonValidPortException::what() const throw() {
	return "Port value should be an integer between 0 and 65535";
}

const char *Exceptions::NonValidCommandException::what() const throw() {
	return "Only valid commands are: KICK, INVITE, TOPIC, MODE.";
}
