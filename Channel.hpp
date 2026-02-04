#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include "Client.hpp"

class Channel {
	private:
	    std::string _name;
	    std::set<Client*> _users;
	    std::set<Client*> _operators;
	public:
	    Channel(const std::string& name);
	    ~Channel();
	
	    void addUser(Client* client);
	    void removeUser(Client* client);
	    bool hasUser(Client* client) const;
	    bool isOperator(Client* client) const;
	    void setOperator(Client* client, bool op);
	    const std::string& getName() const;
	    const std::set<Client*>& getUsers() const;

		void sendToChannel(Channel& channel, const std::string& message, Client* exclude);
};

#endif