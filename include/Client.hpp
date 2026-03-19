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
	std::set<std::string> channels_joined;
    bool _isRegistered;
    bool _isPasswordOk;
    bool isRegisted() const;
    void setRegisted(bool value);
    int getFd() const;
    std::string &getBuffer();
    std::string &getWriteBuffer();

    void setNickname(const std::string &nickname);
    void setUsername(const std::string &username);
    void setMode(const std::string &mode);
    void setUnused(const std::string &unused);
    void setRealname(const std::string &realname);
    std::string getNickname() const;
    std::string &getUsername();
    
private:
    int _fd;
    
    std::string _buffer;
    std::string _writeBuffer;
    std::string _nickname;
    std::string _username;
    std::string _mode;
    std::string _unused;
    std::string _realname;
};

#endif
