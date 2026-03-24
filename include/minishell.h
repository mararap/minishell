/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 15:45:46 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/24 15:09:12 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

# define SHELL_NAME "juma[n]she"
# define PROMPT_STR "juma[n]she$ "
# define HEREDOC_PROMPT "> "

# define MS_MASK_SPACE 0x1F
# define MS_MASK_TAB 0x1E
# define MS_MASK_NL 0x1D

# define REDIR_IN TOKEN_REDIR_IN
# define REDIR_OUT TOKEN_REDIR_OUT
# define REDIR_APPEND TOKEN_REDIR_APPEND
# define REDIR_HEREDOC TOKEN_HEREDOC

extern volatile sig_atomic_t	g_signal_number;

typedef struct s_env_var
{
	char						*name;
	char						*value;
	struct s_env_var			*next;
	int							exported;
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
	char						*value;
	char						*raw;
	t_token_type				type;
	int							quoted;
	struct s_token				*next;
}	t_token;

typedef struct s_redir
{
	int							type;
	char						*target;
	int							ambiguous;
	int							heredoc_fd;
	int							heredoc_expand;
	int							heredoc_line;
	struct s_redir				*next;
}	t_redir;

typedef struct s_command
{
	char						**argv;
	t_redir						*redirections;
	struct s_command			*next;
}	t_command;

typedef struct s_shell
{
	t_env_var					*env_list;
	int							last_exit_status;
	int							is_interactive;
	int							input_line_num;
	int							should_exit;
	pid_t						main_pid;
	char						*current_line;
}	t_shell;

typedef struct s_lexr_state
{
	t_shell				*shell;
	char				*line;
	int					i;
	int					expect_heredoc_delim;
}	t_lexr_state;

typedef struct s_word_ctx
{
	t_shell				*shell;
	char				*str;
	int					*idx;
	int					*was_quoted;
	int					allow_expansion;
}	t_word_ctx;

typedef struct s_exec_ctx
{
	t_shell				*shell;
	t_command			*cmd_list;
	int					prev_read;
	int					pipe_fd[2];
	pid_t				*pids_to_free;
}	t_exec_ctx;

typedef struct s_pipeline
{
	t_shell				*shell;
	t_command			*command_list;
	t_command			*cmd;
	pid_t				*pids;
	int					prev_read;
	int					pipe_fd[2];
	int					created;
}	t_pipeline;

typedef struct s_wait_state
{
	pid_t					last_pid;
	int						interactive;
	int						last_status;
}	t_wait_state;

typedef struct s_hd_buf
{
	char				*buf;
	size_t				len;
	size_t				cap;
}	t_hd_buf;

typedef struct s_hd_job
{
	t_shell				*shell;
	t_command			*cmds;
	t_redir				*redir;
	char				*path;
	int					wfd;
	int					st;
	int					*lines_read;
}	t_hd_job;

void					ms_init_shell(t_shell *shell, char **envp);
void					ms_free_shell(t_shell *shell);

t_env_var				*ms_env_from_environ(char **envp);
void					ms_env_free_list(t_env_var **env_list);
char					**ms_env_to_array(t_env_var *env_list);
char					*ms_env_get_value(t_env_var *env_list, char *name);
int						ms_env_set(t_env_var **env_list, char *name,
							char *value, int exported);
int						ms_env_unset(t_env_var **env_list, char *name);
int						ms_env_is_valid_name(const char *str);

void					ms_main_loop(t_shell *shell);
void					ms_handle_line(t_shell *shell, char *line);
void					ms_chomp_eol(char *s);
void					ms_discard_pending_heredocs(t_token *tokens);
char					*ms_get_next_line(int fd);
int						ms_rl_event_hook(void);

void					ms_setup_interactive_signals(void);
void					ms_setup_child_signals(void);

int						ms_prepare_heredocs(t_shell *shell, t_command *cmds);
void					ms_setup_heredoc_child_signals(void);
void					ms_tok_add_back(t_token **list, t_token *new_tok);
void					ms_print_syntax_error(t_token *tok);
t_redir					*ms_create_redir(int type, char *target,
							int heredoc_expand, int ambiguous);
void					ms_redir_add_back(t_redir **list, t_redir *new_redir);
int						ms_token_to_redir_type(t_token_type type);
t_token					*ms_tok_new(t_token_type type, char *value,
							char *raw, int quoted);
void					ms_mask_ifs(char *s);
int						ms_has_unclosed_quotes(char *line);
t_command				*ms_syntax_error(t_shell *shell);
char					*ms_collect_single_quotes(char *str, int *idx);
char					*ms_collect_double_quotes(t_shell *shell, char *str,
							int *idx, int allow_expansion);
char					*ms_collect_locale_quotes(t_shell *shell, char *str,
							int *idx, int allow_expansion);
char					*ms_expand_variable(t_shell *shell, char *str,
							int *idx);
char					*ms_collect_word(t_word_ctx *wctx);
t_token					*ms_lex_line(t_shell *shell, char *line);
void					ms_free_token_list(t_token *token_list);
t_command				*ms_parse_tokens(t_token *token_list);
char					**ms_split_ifs_fields(const char *s);
void					ms_unmask_ifs(char *s);
void					ms_handle_word(t_command *cmd, t_token **cursor,
							t_token *tok);
int						ms_process_redir_token(t_command *cmd, t_token **cursor,
							t_token *tok);
int						ms_fill_command(t_command *cmd, t_token **cursor);
void					ms_free_command_list(t_command *command_list);

int						ms_execute_pipeline(t_shell *shell,
							t_command *command_list);
int						ms_spawn_pipeline(t_pipeline *pl);
int						ms_wait_for_children(pid_t last_pid, int count,
							int interactive);
int						ms_is_builtin(char *cmd_name);
int						ms_builtin_needs_parent(char *cmd_name);
int						ms_run_builtin_parent(t_shell *shell, t_command *cmd);
int						ms_run_builtin_child(t_shell *shell, char **argv);
int						ms_check_redirections(t_redir *redirections);
int						ms_apply_redirections(t_redir *redirections);
int						ms_fork_and_execute(t_exec_ctx *ctx, t_command *cmd);
void					ms_execute_child(t_exec_ctx *ctx, t_command *cmd);
char					*ms_find_executable(t_shell *shell, char *cmd,
							int *used_path);
void					ms_update_underscore(t_shell *shell, char *value);
void					ms_adjust_envp_shlvl(char **envp);
int						ms_exec_error_code(char *arg, int err_no);
int						ms_exec_precheck(char *argv0, char *display_arg,
							char *path);
int						ms_exec_external_command(t_shell *shell, char **argv);

int						ms_builtin_dot(t_shell *shell, char **argv);
int						ms_builtin_echo(char **argv);
int						ms_builtin_pwd(t_shell *shell);
int						ms_builtin_cd(t_shell *shell, char **argv);
int						ms_builtin_env(t_shell *shell, char **argv);
int						ms_builtin_export(t_shell *shell, char **argv);
int						ms_builtin_unset(t_shell *shell, char **argv);
int						ms_builtin_exit(t_shell *shell, char **argv);
void					ms_cd_output(char *str);
void					ms_print_export_format(t_env_var *env_list);
int						ms_export_one_arg(t_shell *shell, char *arg);
int						ms_atoll_strict(const char *s, long long *out);

void					*ms_xmalloc(size_t size);
char					*ms_strdup_safe(const char *src);
void					ms_print_command_not_found(char *cmd);
size_t					ms_str_arr_len(char **arr);
void					ms_free_str_array(char **arr);
char					*ms_str_join_three(char *a, char *b, char *c);

int						ms_hd_open_tmp(char **out_path);
void					ms_restore_signals(t_shell *shell);
char					*ms_expand_heredoc_line(t_shell *shell, char *line);
char					*ms_hd_read_line(t_shell *shell);
void					ms_hd_warn_eof(t_redir *redir);
int						ms_hd_write_line(t_shell *shell, t_redir *redir,
							int wfd, char *line);
int						ms_hd_child_loop(t_shell *shell, t_redir *redir,
							int wfd);
void					ms_hd_run_child(t_hd_job *job);
int						ms_hd_wait_child(pid_t pid, int *st);
int						ms_hd_finalize_build(t_hd_job *job);
int						ms_build_one_heredoc(t_shell *shell, t_command *cmds,
							t_redir *redir, int *lines_read);
void					ms_close_all_heredocs(t_command *cmds);
int						ms_prepare_one_heredoc(t_shell *shell,
							t_command *cmds, t_redir *redir,
							int *hd_line_num);
char					**ms_split_path_keep_empty(const char *s);

#endif
