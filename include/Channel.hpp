#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include "Client.hpp"
#include "Token.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cerrno>
#include <limits>



class Channel {
public:
    Channel(const std::string& name);
    ~Channel();
	
		//gettersmmode

		bool getInviteMode(void) const;
		bool getTopicMode(void) const;
		int getChannelLimit(void) const;
		std::string getKey(void) const; // or: const std::string& getKey() const;

		void addInvited(Client* client);
	    void addUser(Client* client);
	    void removeUser(Client* client);
	    bool hasUser(Client* client) const;
	    bool isOperator(Client* client) const;
		void setTopic(std::string top);
		std::string getTopic() const;
	    void setOperator(Client* client, bool op);
	    const std::string& getName() const;
	    const std::set<Client*>& getInvited() const;
		const std::set<Client*>& getUsers() const;

		void sendToChannel(const std::string& message, Client* exclude);

		//COMMANDS

		void	commandHub(Token tok, Client *client);
		void 	sendMessage(Client* sender, const std::string& message);

	private:
	    std::string _name;
	    std::set<Client*> _users;
	    std::set<Client*> _operators;
		std::set<Client*> _invited;

		std::string _topic;
		std::string _key;
		bool	_inviteOnly;
		bool	_topicRestricted;
		int		_channelLimit;


};

#endif