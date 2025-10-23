#include "Ircserv.hpp"


int main(int argc, char** argv) {
    
    if (argc != 3) {
        throw Exceptions::ExceptionParams();
        return 1;
    }
    (void)argv;
    return 0;
}