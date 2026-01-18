#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

# include "libft.h"

/*
** Macros
*/

# define SHELL_NAME "jumanshe"
# define PROMPT_STR "jumanshe$ "
# define HEREDOC_PROMPT "> "

# define REDIR_IN 0
# define REDIR_OUT 1
# define REDIR_APPEND 2
# define REDIR_HEREDOC 3

/*
** Global used only for signal number (required by subject).
** The rest of the state stays inside t_shell.
*/

//extern int	g_signal_number;
extern volatile sig_atomic_t	g_signal_number;

/*
** Data structures
*/

typedef struct s_env_var
{
	char				*name;
	char				*value;
	struct s_env_var	*next;
}	t_env_var;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	int				type;
	char			*target;
	struct s_redir	*next;
}	t_redir;

typedef struct s_command
{
	char				**argv;
	t_redir				*redirections;
	struct s_command	*next;
}	t_command;

typedef struct s_shell
{
	t_env_var	*env_list;
	int			last_exit_status;
	int			is_interactive;
}	t_shell;

/*
** main / init / cleanup
*/

void		ms_init_shell(t_shell *shell, char **envp);
void		ms_free_shell(t_shell *shell);

/*
** env list helpers
*/

t_env_var	*ms_env_from_environ(char **envp);
void		ms_env_free_list(t_env_var *env_list);
char		**ms_env_to_array(t_env_var *env_list);
char		*ms_env_get_value(t_env_var *env_list, char *name);
int			ms_env_set(t_env_var **env_list, char *name, char *value);
int			ms_env_unset(t_env_var **env_list, char *name);

/*
** loop / line handling
*/

void		ms_main_loop(t_shell *shell);
void		ms_handle_line(t_shell *shell, char *line);

/*
** signals
*/

void		ms_setup_interactive_signals(void);
void		ms_setup_child_signals(void);

/*
** lexer / parser
*/

t_token		*ms_lex_line(t_shell *shell, char *line);
void		ms_free_token_list(t_token *token_list);
t_command	*ms_parse_tokens(t_token *token_list);
void		ms_free_command_list(t_command *command_list);

/*
** executor / redirections
*/

int			ms_execute_pipeline(t_shell *shell, t_command *command_list);
int			ms_is_builtin(char *cmd_name);
int			ms_run_builtin_parent(t_shell *shell, t_command *cmd);
int			ms_run_builtin_child(t_shell *shell, char **argv);
int			ms_apply_redirections(t_redir *redirections, t_shell *shell);
int			ms_fork_and_execute(t_shell *shell, t_command *cmd,
	int prev_read, int pipe_fd[2]);

/*
** builtins
*/

int			ms_builtin_echo(char **argv);
int			ms_builtin_pwd(t_shell *shell);
int			ms_builtin_cd(t_shell *shell, char **argv);
int			ms_builtin_env(t_shell *shell, char **argv);
int			ms_builtin_export(t_shell *shell, char **argv);
int			ms_builtin_unset(t_shell *shell, char **argv);
int			ms_builtin_exit(t_shell *shell, char **argv);

/*
** utils
*/

void		*ms_xmalloc(size_t size);
char		*ms_strdup_safe(const char *src);
void		ms_perror(char *msg);
void		ms_print_command_not_found(char *cmd);
size_t		ms_str_arr_len(char **arr);
void		ms_free_str_array(char **arr);
char		*ms_str_join_three(char *a, char *b, char *c);

#endif //!MINISHELL_H
