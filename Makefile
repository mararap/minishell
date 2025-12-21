# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/20 21:31:57 by marapovi          #+#    #+#              #
#    Updated: 2025/12/21 20:21:47 by marapovi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                 VARIABLES                                    #
# **************************************************************************** #

NAME		:=	minishell	

HEADER		:=	./include/minishell.h

CC			:=	cc

# C PreProcessor flags - exclusively needed when compiling src files
CPPFLAGS	:=	-I include -I libft

# C compiler flags - needed for compiling src files AND for linking
# CFLAGS		:=	-Wall -Wextra -Werror -O2 -march=native -g
CFLAGS		:=	-Wall -Wextra -Werror

# Flags/Options passed to the linker
LDFLAGS		:=	-L libft

# Libraries to pass to the linker
LDLIBS 		:=	-lft -lreadline

# set RM to remove directories and containing files recursiveley
RM			:=	rm -rf

# UNIX archive utility for creating static library
#	c to explicitly create the library and silence warning if its not there
#	r to replace existing symbols (functions) in the library and/or add new
#	s to create an index for the library so linker will find symbols quickly
#A	R		:=	ar crs


# **************************************************************************** #
#                                   PATHS                                      #
# **************************************************************************** #

OBJ_DIR			:=		obj
SRC_DIR			:=		src
LIBFT_DIR		:=		libft
LIBFT			:=		$(LIBFT_DIR)/libft.a

SRC				:=		main.c\
						builtin_cd.c\
						builtin_env_export_unset.c\
						builtin_exit.c\
						builtins.c\
						builtins_echo.c\
						builtins_pwd.c\
						env_list.c\
						executor.c\
						init.c\
						lexer/lexer.c\
						loop.c\
						parser/parser.c\
						redirections.c\
						signals.c\
						utils.c


SRC				:=		$(addprefix $(SRC_DIR)/,$(SRC))
OBJ				:=		$(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: libft_always $(NAME)

.PHONY: libft_always

libft_always:
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR);

vpath %.c $(SRC_DIR) $(SRC_DIR)/lexer $(SRC_DIR)/parser

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ): %.o: $(HEADER)

$(NAME): $(OBJ) $(LIBFT)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(NAME)
	@echo "     ✅ MINISHELL executable successfully created."
	

clean:
	@$(RM) $(OBJ_DIR)
	@echo "     🧽 MIINISHELL	= clean."
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean
	@echo "     🧽 LIBFT 		= clean."

fclean: clean
	@$(RM) $(NAME)
	@echo "     🧹 MINISHELL executable removed."
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean
	@echo "     🧹 LIBFT library removed."

re: fclean all

.PHONY: all clean fclean re%  
