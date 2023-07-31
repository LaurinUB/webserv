NAME =	webserv
CXX =	c++
CXXFLAGS =	-std=c++98 -Wall -Wextra -Werror -g #-fsanitize=address

GREEN =	\033[0;32m
CYAN =	\033[0;36m
RED =	\033[0;31m
WHITE =	\033[0;0m

###############################################################################
##   FILESTRUCTURE MANAGEMENT   ###############################################
###############################################################################

##   Source file handling
SRC_DIR = src/
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
SRC_FILES = HTTPRequest.cpp HTTPResponse.cpp TcpServer.cpp main.cpp Socket.cpp
ALL_SRC = $(SRC)

##   Object file handling
OBJ_DIR = obj/
ALL_OBJ = $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(ALL_SRC))
CREATE_OBJ_DIR = $(sort $(dir $(ALL_OBJ)))

###############################################################################
##   MAKEFILE RULES   #########################################################
###############################################################################

all:	$(NAME)

$(NAME): $(CREATE_OBJ_DIR) $(ALL_OBJ)
	@$(CXX) $(ALL_OBJ) -o $(NAME)
	@echo "$(GREEN)Compiled successfully$(WHITE)"

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp
	@echo "$(CYAN)Compiling$(WHITE): $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

run:	all
	@./$(NAME)

clean:
	@echo "$(RED)rm -f $(OBJ)$(WHITE)"
	@rm -f $(ALL_OBJ)

fclean:	clean
	@echo "$(RED)rm -f $(NAME)$(WHITE)"
	@rm -f $(NAME)

re:	fclean all

$(CREATE_OBJ_DIR):
	@mkdir -p $(CREATE_OBJ_DIR)

.PHONY: all $(NAME) run clean fclean re
