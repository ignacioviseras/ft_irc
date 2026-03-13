#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include "Token.hpp"
#include <cstdlib>

std::vector<std::string> split(const std::string& s, const std::string& delimiter);
bool parse_commands(const std::string& input);
bool valid_server_port(const char *arg);
Token::type Token_assign_type(const std::string& arg);
std::string normalizeChannelName(const std::string& rawName);
bool isValidChannelName(const std::string& channelName);

#endif