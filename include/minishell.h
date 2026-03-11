#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <limits.h>
# include <sys/stat.h>

/*
** Macros
*/

# define SHELL_NAME "juma[n]she"
# define PROMPT_STR "juma[n]she$ "
# define HEREDOC_PROMPT "> "

# define REDIR_IN TOKEN_REDIR_IN
# define REDIR_OUT TOKEN_REDIR_OUT
# define REDIR_APPEND TOKEN_REDIR_APPEND
# define REDIR_HEREDOC TOKEN_HEREDOC

/*
** Global used only for signal number (required by subject).
** The rest of the state stays inside t_shell.
*/

// extern int	g_signal_number;
extern volatile sig_atomic_t	g_signal_number;

/*
** Data structures
*/

typedef struct s_env_var
{
	char						*name;
	char						*value;
	struct s_env_var			*next;
	int							exported;
}								t_env_var;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}								t_token_type;

typedef struct s_token
{
	char						*value;
	t_token_type				type;
	int quoted; /* NEW: 1 if this word contained any quotes */
	struct s_token				*next;
}								t_token;

typedef struct s_redir
{
	int							type;
	char						*target;
	int 						heredoc_fd;     /* NEW: read-fd prepared before exec */
	int 						heredoc_expand; /* NEW: 1 if expand heredoc lines */
	int							heredoc_line; /* line where heredoc starts*/
	struct s_redir				*next;
}								t_redir;

typedef struct s_command
{
	char						**argv;
	t_redir						*redirections;
	struct s_command			*next;
}								t_command;

typedef struct s_shell
{
	t_env_var					*env_list;
	int							last_exit_status;
	int							is_interactive;
	int							input_line_num;
	int							should_exit;
	pid_t						main_pid;
	char						*current_line;
}								t_shell;

/*
** main / init / cleanup
*/

void							ms_init_shell(t_shell *shell, char **envp);
void							ms_free_shell(t_shell *shell);

/*
** env list helpers
*/

t_env_var						*ms_env_from_environ(char **envp);
void							ms_env_free_list(t_env_var **env_list);
char							**ms_env_to_array(t_env_var *env_list);
char							*ms_env_get_value(t_env_var *env_list,
									char *name);
int								ms_env_set(t_env_var **env_list, char *name,
									char *value, int exported);
int								ms_env_unset(t_env_var **env_list, char *name);

/*
** loop / line handling
*/

void							ms_main_loop(t_shell *shell);
void							ms_handle_line(t_shell *shell, char *line);
char							*ms_get_next_line(int fd);
void							ms_setup_interactive_signals(void);
int								ms_rl_event_hook(void);

/*
** signals
*/

void							ms_setup_interactive_signals(void);
void							ms_setup_child_signals(void);

/*
** lexer / parser
*/

int								ms_prepare_heredocs(t_shell *shell,
									t_command *cmds);
void							ms_token_add_back(t_token **list,
									t_token *new_tok);
t_token							*ms_token_new(t_token_type type, char *value,
									int quoted);
char							*ms_collect_word(t_shell *shell, char *str,
									int *idx, int *was_quoted,
									int allow_expansion);
t_token							*ms_lex_line(t_shell *shell, char *line);
void							ms_free_token_list(t_token *token_list);
t_command						*ms_parse_tokens(t_token *token_list);
int								ms_fill_command(t_command *cmd,
									t_token **cursor);
void							ms_free_command_list(t_command *command_list);

/*
** executor / redirections
*/

int								ms_execute_pipeline(t_shell *shell,
									t_command *command_list);
int								ms_is_builtin(char *cmd_name);
int								ms_run_builtin_parent(t_shell *shell,
									t_command *cmd);
int								ms_run_builtin_child(t_shell *shell,
									char **argv);
int								ms_check_redirections(t_redir *redirections);
int								ms_apply_redirections(t_redir *redirections);
int								ms_fork_and_execute(t_shell *shell,
									t_command *cmd_list, t_command *cmd, int prev_read,
									int pipe_fd[2]);
void							ms_execute_child(t_shell *shell, t_command *cmd_list,
									t_command *cmd,
									int in_fd, int out_fd);

/*
** builtins
*/
int								ms_builtin_dot(t_shell *shell, char **argv);
int								ms_builtin_echo(char **argv);
int								ms_builtin_pwd(t_shell *shell);
int								ms_builtin_cd(t_shell *shell, char **argv);
int								ms_builtin_env(t_shell *shell, char **argv);
int								ms_builtin_export(t_shell *shell, char **argv);
int								ms_builtin_unset(t_shell *shell, char **argv);
int								ms_builtin_exit(t_shell *shell, char **argv);

/*
** utils
*/

void							*ms_xmalloc(size_t size);
char							*ms_strdup_safe(const char *src);
void							ms_perror(char *str, int err_no);
void							ms_print_command_not_found(char *cmd);
size_t							ms_str_arr_len(char **arr);
void							ms_free_str_array(char **arr);
char							*ms_str_join_three(char *a, char *b, char *c);

#endif //! MINISHELL_H
