NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS =	src/main.cpp \
		src/Exceptions.cpp \
		src/Server.cpp \
		src/Token.cpp \
		src/Utils.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/commands/user.cpp \
		src/commands/nick.cpp \
		src/commands/pass.cpp \
		src/commands/part.cpp \
		src/commands/join.cpp \
		src/commands/invite.cpp \
		src/commands/names.cpp \
		src/commands/privmsg.cpp \
		src/commands/kick.cpp \
		src/commands/quit.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re