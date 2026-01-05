#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
public:
    Client(int fd);
    ~Client();

    int getFd() const;
    std::string &getBuffer();
    bool isRegisted() const;
    void setRegisted(bool value);

private:
    int _fd;
    std::string _buffer;
    bool _registered;
};

#endif
