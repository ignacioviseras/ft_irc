#include "Ircserv.hpp"


int main(int argc, char** argv) {
    
    try
    {
        if (argc != 3)
            throw Exceptions::ExceptionParams();

        (void)argv;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}