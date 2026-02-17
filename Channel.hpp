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
	private:
	    std::string _name;
	    std::set<Client*> _users;
	    std::set<Client*> _operators;
		std::string _topic;
		std::string _key;
		bool	_inviteOnly;
		bool	_topicRestricted;
		int		_channelLimit;


	public:
	    Channel(const std::string& name);
	    ~Channel();
	
	    void addUser(Client* client);
	    void removeUser(Client* client);
	    bool hasUser(Client* client) const;
	    bool isOperator(Client* client) const;
		void setTopic(std::string top);
		std::string getTopic();
	    void setOperator(Client* client, bool op);
	    const std::string& getName() const;
	    const std::set<Client*>& getUsers() const;
		void sendToChannel(Channel& channel, const std::string& message, Client* exclude);

		//COMMANDS

		void	commandHub(Token tok, Client *client);
		void	commandKick(Client *client);
		void	commandInvite(Client *client);
		void	commandTopic(const std::vector<std::string>& args);
		void	commandTopic(Client *client);
		void    commandMode(const std::vector<std::string>& args);

		void    commandModeInvite();
		void    commandModeKey(const std::vector<std::string>& args);
		void    commandModeOperator(const std::vector<std::string>& args);
		void    commandModeTopic();
		void    commandModeLimit(const std::vector<std::string>& args);


	};

#endif