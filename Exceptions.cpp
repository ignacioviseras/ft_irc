#include "Exceptions.hpp"

const char *Exceptions::ExceptionParams::what() const throw() {
	return "The program need 3 params to execute";
}

