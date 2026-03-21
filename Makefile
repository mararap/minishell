# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/20 21:31:57 by marapovi          #+#    #+#              #
#    Updated: 2026/03/18 19:11:50 by marapovi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                 VARIABLES                                    #
# **************************************************************************** #

NAME		:=	minishell	

HEADER		:=	./include/minishell.h

define MONKEY
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⢀⠀⠀⠀⠀⠀⠀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣷⣿⣿⣿⣿⣿⣿⣿⡷⢶⣾⣿⣿⡶⣵⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣶⣶⣿⣿⣿⢿⡏⣿⡈⣯⠙⠛⠝⠙⢣⣼⣿⠿⣿⣿⣿⣿⣶⣶⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⣾⣿⣿⣯⣿⢿⡯⢎⣳⢼⣂⡹⣆⠀⠀⣠⡿⠛⢠⠶⠋⠋⣦⡿⣿⣟⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⣯⠭⢯⣉⠉⠉⠛⢿⣶⣽⣾⣿⠟⢮⡳⠰⢣⣤⣾⣯⢖⡶⠚⢙⣋⠉⣭⣭⢻⣿⣿⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣶⣿⣉⣿⡿⠁⠀⠀⠀⠀⠤⠤⣼⡿⠤⠤⠀⢀⢠⢠⣆⡤⢏⡠⠾⣿⣇⡀⠀⠀⠁⠈⠁⢿⣿⣿⣿⣷⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⣤⣦⣼⣿⣿⣿⣿⠥⠀⠐⠒⠛⠛⠚⢛⡿⠧⠀⠀⠀⠈⠀⠀⠀⠶⠶⢶⡛⠽⠟⠂⠒⠖⠂⣤⡤⠴⣿⣿⣿⣿⣧⣤⣤⠤⠄⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣀⣠⣀⠀⠀⠀⢻⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠉⢰⣯⠤⠄⠢⢤⡀⠀⣀⣀⠈⠁⠢⠤⠦⣭⣭⣿⣢⡤⠤⠄⠉⠀⣸⣿⣿⣿⣯⡏⠀⠀⠀⣀⣰⡀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣿⡿⠟⣿⣶⣄⣀⣿⣿⣿⣟⡂⢀⣀⣀⣀⣥⣾⣏⣀⣠⠒⠀⣀⠀⠀⠐⠂⠀⠀⠀⢀⣀⣠⣤⣽⣿⣯⣲⣤⣀⣀⣬⣿⣿⣿⣿⣄⣠⣴⣾⣻⣿⠿⡇⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⡇⢠⣿⣿⣿⣿⣿⣻⣿⣿⡃⠀⣤⣤⣽⡿⠛⠉⠉⠀⠀⠀⠀⠉⠉⣥⡀⠀⢉⡭⠔⠋⠀⠀⠀⠀⠉⠉⠻⣿⣿⣤⣭⣟⣿⣿⣿⣿⣿⣿⣿⠿⣁⣠⠃⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⡇⢻⣍⠩⣾⣀⠤⣾⣿⡿⠽⢿⣿⣿⠏⠀⠀⠀⠀⠀⢀⣠⣂⢀⠀⢠⡇⢸⠃⠀⢀⣄⢠⣀⠀⠀⠀⠀⣀⡈⣿⣿⣾⣿⣿⣿⣿⡤⣀⣳⠬⣄⣈⣹⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⢳⡉⠛⠘⣿⣿⠟⡻⠿⠚⠿⣿⣿⣿⣀⠀⢀⡴⣾⣿⣿⣾⣿⣿⣾⣿⣇⣿⣾⣾⣿⣿⣿⣖⣚⣿⡿⡿⠀⢠⣹⣿⣿⡿⣿⣿⣿⣻⣿⣿⠉⠛⢁⡿⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠙⢷⡀⠀⠒⠋⠒⡀⡄⢂⣶⣿⣿⣿⣇⣀⣿⣿⣿⡽⢛⣿⣿⣿⣿⣿⢅⣻⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⣦⣬⣿⣿⣿⣷⣾⠿⣿⣇⠑⠈⠀⢀⣸⠛⠁⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠈⢧⡀⠀⠰⢶⣿⣶⣯⣼⣿⣿⣿⢯⡭⣿⠟⠋⠑⢦⡼⢿⠼⢙⣯⠈⣭⣿⣿⡏⠋⠘⢯⠽⣿⢟⡿⠯⢭⣿⣿⣿⣿⣯⣳⣿⣯⠦⠈⢈⡿⠃⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢠⠼⠛⣦⣤⠝⢯⡟⢠⢷⣿⣿⣿⡋⠀⠀⠀⠀⠀⠀⠀⠉⣩⣞⣤⣤⣾⣭⣭⣍⢦⠀⠀⠀⠈⠁⠀⢀⡍⣻⣿⣿⡿⣗⢿⣿⡗⢠⣠⠾⠧⠄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢠⣶⣿⣷⠀⣿⡷⠃⣼⣿⣿⣿⡟⠀⠀⠀⠠⢀⣠⠶⣫⣿⡿⢧⣄⠀⣀⡬⢷⣿⡷⣄⡲⠀⠀⠀⠀⢰⣾⣿⣿⣷⠘⢷⣿⡅⢸⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢠⡾⠟⠋⡹⢷⣿⣥⡞⢁⡟⣿⣿⣗⢠⣾⠖⠈⢀⡴⣾⡟⠁⡄⠀⠘⠟⠁⠀⢀⠉⢿⣦⣈⣓⡀⣀⢀⣠⣿⣿⡏⠣⠀⢿⣿⣿⣌⠙⠻⠿⣄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠏⠀⢀⣼⡁⠀⢠⣏⣠⠞⣽⣿⣿⣿⣿⣷⡾⣻⣯⣾⡿⠀⢠⣇⠀⠀⠀⠀⢀⣼⣤⠾⣿⣮⣿⠿⣿⣿⣿⣿⣿⣻⡆⠀⠘⣿⣿⣟⣷⣄⠀⠘⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣉⣭⣷⣦⣾⣿⣃⣴⣿⣿⣿⡿⣩⣯⡤⢿⣿⠏⠀⠀⠈⣛⠾⢦⢠⣿⡿⠟⠁⠀⠸⡿⣷⡖⠊⣿⣿⣿⣿⢿⣧⣘⣾⣿⣿⣿⣯⣝⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣀⡴⠿⢿⣿⣿⣿⣿⣯⡾⢹⣿⣿⣿⣿⠁⣬⡿⠁⠀⠀⠀⠀⠀⠑⠂⠘⠃⠀⠀⠀⢀⣀⠀⠹⣯⡀⣽⣿⣿⣿⠀⢻⣿⣿⣿⣿⣿⣿⠿⢷⣄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⢠⠶⠾⠿⢿⣏⣿⣿⣿⣿⣿⣿⣧⢸⣿⣿⣿⣿⣴⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢆⠻⣠⠀⠙⣷⣾⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣯⣿⣿⣷⡶⠖⠀⠀⠀⠀
⠀⠀⠀⣀⣥⠴⠾⡛⠉⠉⠛⠇⠈⣉⣿⣿⣿⣼⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡟⣯⣆⠀⠘⣿⣿⢙⣟⢿⣾⣿⣿⣏⡉⠁⠘⠉⠉⠙⢿⣦⣤⣄⡀⠀
⠀⠐⣫⠉⣠⣴⠊⠁⠀⠀⠀⠀⠀⠙⠛⢿⣿⣿⣴⣿⣹⠿⡄⠀⠀⠐⢆⣠⡀⠀⠀⠀⠀⠀⠀⠀⢀⣶⣾⣿⣿⣷⠂⠀⢾⣿⣾⣟⣿⣿⣿⠛⠁⠀⠀⠀⠀⠀⠀⠀⢿⣧⡉⠹⠳
⠀⣠⣥⢾⡏⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⡿⣿⣾⡷⠀⠐⣠⡀⠈⠁⠀⠀⠀⠀⠀⠀⠲⠟⠻⠚⠋⢳⣟⡀⢠⣿⣿⣿⣿⣿⣿⣇⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣦⣄
⢈⣩⣷⠿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⣿⣷⣿⣿⣽⡇⠈⠁⠉⢑⣲⣶⡶⣶⣶⣶⣶⣶⣶⣶⣶⣻⣭⡶⠀⣸⣿⣿⣿⣿⣿⡟⠿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠺⣿⠯
⢐⣫⣤⡦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠉⠙⠋⠛⠙⠟⠻⣿⣿⣿⡟⠀⠀⣿⣿⣿⣿⣿⣿⡧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀
⠈⠹⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⢿⣿⣿⣿⣿⣷⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⣤⣬⣿⡿⠋⠀⠀⣼⣿⣿⣿⣟⠙⢿⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠢
⢨⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠈⠈⢏⣿⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠰⠖⠊⠉⠁⠀⠀⣠⣿⣿⣿⣿⠋⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢬
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣙⣿⣿⣿⣿⣿⣇⣄⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢙⣿⣿⣿⣿⣿⣿⣿⣦⣄⣰⣴⣶⣰⣾⣿⣿⣿⣿⡿⣿⢿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⡋⡇⡻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⠟⠧⣸⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣽⣧⠃⣸⡛⠻⢻⣿⡿⠻⣿⣿⢿⢿⠋⠀⠀⣾⣙⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀

        _                       ___     ___     _          
       (_)                     |  _|   |_  |   | |         
        _ _   _ _ __ ___   __ _| | _ __  | |___| |__   ___ 
       | | | | | '_ ` _ \ / _` | || '_ \ | / __| '_ \ / _ \ 
       | | |_| | | | | | | (_| | || | | || \__ \ | | |  __/ 
       | |\__,_|_| |_| |_|\__,_| ||_| |_|| |___/_| |_|\___|
      _/ |                     |___|   |___|               
     |__/                                                  

   ✅ Minishell 'juma[n]she' by Juliyan and Maria is now ready.

endef
export MONKEY

CC			:=	cc

# C PreProcessor flags - exclusively needed when compiling src files
CPPFLAGS	:=	-I include -I libft

# C compiler flags - needed for compiling src files AND for linking
# CFLAGS		:=	-Wall -Wextra -Werror -O2 -march=native -g
CFLAGS		:=	-Wall -Wextra -Werror -g

# Flags/Options passed to the linker
LDFLAGS		:=	-L libft

# Libraries to pass to the linker
LDLIBS 		:=	-lft -lreadline

# set RM to remove directories and containing files recursiveley
RM			:=	rm -rf

# UNIX archive utility for creating static library
# c to explicitly create the library and silence warning if its not there
# r to replace existing symbols (functions) in the library and/or add new
# s to create an index for the library so linker will find symbols quickly
# AR		:=	ar crs


# **************************************************************************** #
#                                   PATHS                                      #
# **************************************************************************** #

OBJ_DIR			:=		obj
SRC_DIR			:=		src
LIBFT_DIR		:=		libft
LIBFT			:=		$(LIBFT_DIR)/libft.a

SRC 	:= 		main.c \
				init.c \
				loop.c \
				loop_syntax.c \
				signals.c \
				utils.c \
				utils_command.c \
				utils_perror.c \
				utils_gnl.c \
				env_list.c \
				env_list_ops.c \
				env_list_get.c \
				env_list_unset.c \
				env_list_array.c \
				redirections.c \
				heredoc_tmp.c \
				heredoc_expand.c \
				heredoc_read.c \
				heredoc_build.c \
				heredoc_prepare.c \
				lexer/lexer.c \
				lexer/collect_word.c \
				lexer/collect_word_quotes.c \
				lexer/token_utils.c \
				expand/ifs_split.c \
				expand/expand_variable.c \
				parser/parser.c \
				parser/parser_helpers.c \
				parser/fill_command.c \
				parser/handle_words.c \
				parser/handle_redirs.c \
				parser/free_command.c \
				exec/executor.c \
				exec/executor_wait.c \
				exec/executor_spawn.c \
				exec/exec_path.c \
				exec/exec_error.c \
				exec/exec_run.c \
				exec/fork.c \
				builtin/builtins.c \
				builtin/builtin_cd.c \
				builtin/builtin_cd_output.c \
				builtin/builtin_echo.c \
				builtin/builtin_env.c \
				builtin/builtin_exit.c \
				builtin/builtin_exit_parse.c\
				builtin/builtin_export.c \
				builtin/builtin_export_utils.c \
				builtin/builtin_export_print.c \
				builtin/builtin_pwd.c \
				builtin/builtin_unset.c

SRC				:=		$(addprefix $(SRC_DIR)/,$(SRC))
OBJ				:=		$(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: libft_always print_compile $(NAME)

.PHONY: libft_always print_compile print_done

print_compile:
	@echo "     🛠️  Compiling sources..."

print_done:
	@echo "     ✅ Compilation finished."

libft_always:
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR);

vpath %.c $(SRC_DIR) $(SRC_DIR)/lexer $(SRC_DIR)/parser $(SRC_DIR)/exec $(SRC_DIR)/builtin $(SRC_DIR)/expand

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ): %.o: $(HEADER)

$(NAME): print_done $(OBJ) $(LIBFT)
	@echo "     🛠️  Linking executable..."
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LDLIBS) -o $(NAME)
	@printf "%b\n" "$$MONKEY"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "     🧽 MINISHELL	= clean."
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) clean
	@echo "     🧽 LIBFT 		= clean."

fclean: clean
	@$(RM) $(NAME)
	@echo "     🧹 MINISHELL executable removed."
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR) fclean
	@echo "     🧹 LIBFT library removed."

re: fclean all

.PHONY: all clean fclean re 
