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
    void setHostname(const std::string &hostname);
    void setServername(const std::string &servername);
    void setRealname(const std::string &realname);
    std::string &getNickname();
    std::string &getUsername();
    
private:
    int _fd;
    
    std::string _buffer;
    std::string _writeBuffer;
    std::string _nickname;
    std::string _username;
    std::string _hostname;
    std::string _servername;
    std::string _realname;
};

#endif
