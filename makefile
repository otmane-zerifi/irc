
NAME = ircserv
COMPI = c++ -std=c++98
ERRORS = #-Wall -Wextra -Werror -g

CPP_FILES = *.cpp

all : $(NAME)

$(NAME) : $(CPP_FILES)
	$(COMPI) $(ERRORS) $(CPP_FILES) -o $(NAME)

clean :
	rm -rf $(NAME)

fclean : clean

re : clean all

.PHONY : all clean fclean re