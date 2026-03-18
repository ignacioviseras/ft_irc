# IRC

`drestep` `mmartine` `igvisera`

## Description

We are replicating the operation of Internet Relay Chat (IRC) using C++98. The server must be able to manage multiple clients simultaneously and without blocking (non-blocking)

### Technical description

`ft_irc`is an event-based server that uses `epoll`for multiple input/output (I/O) connections. It allows users to connect either through standard IRC clients (HexChat) or through raw TCP connection tools such as `nc`(Netcat). Once connected, users can register on the network, change their nickname, join chat channels, and send private or global messages to other users in real time.

## Instructions

Server Terminal

```c
make && ./ircsev 5555 passwd
```

Client Terminal

```c
nc localhost 5555
```

| Command | Description |
| --- | --- |
| PASS \<password> | Enter the password to access the server. |
| USER \<username> 0 * \<realname> | Register your username and real name in the system. |
| NICK \<nickname> | We assign what nickname we will have. |
| PRIVMSG \<receptor> \<mensaje> | Send a private message to a user or an entire channel. |
| JOIN \<canal>[,\<canal>] | Allows you to join one or more channels |
| INVITE \<usuario> \<canal> | Invite a user to a channel (required if the channel is `+i`). |
| TOPIC \<canal> [:\<topic>] | Check or change the channel's conversation topic. |
| KICK \<canal> \<usuario> [:\<razón>] | Kick a user from a channel. |
| PART \<canal> [:\<mensaje>] | You leave a specific channel, optionally with a goodbye message. |
| MODE \<canal/user> \<modos> [args] | Change permissions or states. |
| NAMES [\<canal>] | List the nicknames of all users connected to a channel |
| QUIT [:\<mensaje>] | Close your connection to the server permanently. |

## Resources

Documentation by [ircdoc](https://modern.ircdocs.horse/), gemini for errors in compilation or in the operation of the code.