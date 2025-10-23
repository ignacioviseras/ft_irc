NAME = ircserv
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS =	main.cpp \
		ircserv.cpp \
		exceptions.cpp \
		
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