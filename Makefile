# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/20 21:31:57 by marapovi          #+#    #+#              #
#    Updated: 2026/03/23 10:57:39 by jatanaso         ###   ########.fr        #
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
CFLAGS		:=	-Wall -Wextra -Werror

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

SRC 	:= 		shell/main.c \
				shell/init.c \
				shell/loop.c \
				shell/loop_syntax.c \
				shell/signals.c \
				utils/utils.c \
				utils/utils_command.c \
				utils/utils_gnl.c \
				env/env_list.c \
				env/env_list_ops.c \
				env/env_list_get.c \
				env/env_list_unset.c \
				env/env_list_array.c \
				redirections.c \
				heredoc/heredoc_tmp.c \
				heredoc/heredoc_expand.c \
				heredoc/heredoc_read.c \
				heredoc/heredoc_build.c \
				heredoc/heredoc_prepare.c \
				lexer/lexer.c \
				lexer/collect_word.c \
				lexer/collect_word_quotes.c \
				lexer/unclosed_quotes.c \
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
				builtin/builtin_is_builtin.c \
				builtin/builtin_unset.c

SRC				:=		$(addprefix $(SRC_DIR)/,$(SRC))
OBJ				:=		$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: libft $(NAME)

.PHONY: all clean fclean re libft

libft:
	@$(MAKE) --no-print-directory -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJ) $(LIBFT)
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
