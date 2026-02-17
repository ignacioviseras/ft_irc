#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
public:
    Client();
    Client(int fd);
    ~Client();
	std::set<std::string> channels_operating;
    bool _isRegistered;
    bool _isPasswordOk;
    bool isRegisted() const;
    void setRegisted(bool value);
    int getFd() const;
    std::string &getBuffer();

    void setNickname(const std::string &nickname);
    void setUsername(const std::string &username);
    std::string &getNickname();
    std::string &getUsername();
    
private:
    int _fd;
    
    std::string _buffer;
    std::string _nickname;
    std::string _username;
};

#endif
