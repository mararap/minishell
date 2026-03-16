#include "minishell.h"

static char	*ms_cmd_from_cwd(char *cmd)
{
	char	*cwd;
	char	*candidate;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (NULL);
	candidate = ms_str_join_three(cwd, "/", cmd);
	free(cwd);
	return (candidate);
}
static char	*ms_join_search_dir(char *dir, char *cmd)
{
	if (dir[0] == '\0')
		return (ms_str_join_three(".", "/", cmd));
	return (ms_str_join_three(dir, "/", cmd));
}

static int ms_path_match_kind(char *candidate)
{
	struct stat st;

	if (stat(candidate, &st) != 0)
		return (0);
	if (S_ISDIR(st.st_mode))
		return (0);
	if (access(candidate, X_OK) == 0)
		return (2);
	return (1);
}

static char	*ms_pick_path_hit(char *candidate, char **fallback)
{
	int kind;

	kind = ms_path_match_kind(candidate);
	if (kind == 2)
		return (candidate);
	if (kind == 1 && !*fallback)
		*fallback = ft_strdup(candidate);
	free(candidate);
	return (NULL);
}

static char	*ms_return_path_match(char **paths, char *fallback, char *candidate, int *used_path)
{
	ms_free_str_array(paths);
	if (used_path)
		*used_path = 1;
	free (fallback);
	return (candidate);
}

static char	*ms_search_path_dirs(char **paths, char *cmd, int *used_path)
{
	char		*candidate;
	struct stat	st;
	int			i;

	i = 0;
	fallback = NULL;
	while (paths[i])
	{
		candidate = ms_join_search_dir(paths[i], cmd);
		if (access(candidate, F_OK) == 0
			&& stat(candidate, &st) == 0
			&& !S_ISDIR(st.st_mode))
		{
			ms_free_str_array(paths);
			if (used_path)
				*used_path = 1;
			return (candidate);
		}
		free(candidate);
		i++;
	}
	ms_free_str_array(paths);
	if (used_path && fallback)
		*used_path = 1;
	return (fallback);
}

static char	*ms_find_executable(t_shell *shell, char *cmd, int *used_path)
{
	char		*path_env;
	char		**paths;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (used_path)
		*used_path = 0;
	// If the command contains '/', treat it as a relative or absolute path
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	// Get PATH from environment
	path_env = ms_env_get_value(shell->env_list, "PATH");
	// If PATH is unset, bash still checks CWD
	if (!path_env)
		return (ms_cmd_from_cwd(cmd));
	if (path_env[0] == '\0')
		return (ft_strdup(cmd));
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (ms_search_path_dirs(paths, cmd, used_path));
}

static int	ms_exec_exit_code(int err_no)
{
	if (err_no == EISDIR || err_no == ENOEXEC || err_no == EACCES)
		return (126);
	return (127);
}
static void	ms_write_exec_message(int err_no)
{
	if (err_no == ENOENT)
		write(STDERR_FILENO, "No such file or directory\n", 26);
	else if (err_no == EISDIR)
		write(STDERR_FILENO, "Is a directory\n", 15);
	else if (err_no == ENOEXEC)
		write(STDERR_FILENO, "Exec format error\n", 18);
	else if (err_no == EACCES)
		write(STDERR_FILENO, "Permission denied\n", 18);
	else
	{
		write(STDERR_FILENO, strerror(err_no), ft_strlen(strerror(err_no)));
		write(STDERR_FILENO, "\n", 1);
	}
}
static int	ms_exec_error_code(char *arg, int err_no)
{
	if (err_no == 0)
		err_no = ENOENT;
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": ", 2);
	ms_write_exec_message(err_no);
	return (ms_exec_exit_code(err_no));
}

static void	ms_update_underscore(t_shell *shell, char *value)
{
	if (!shell || !value || value[0] == '\0')
		return ;
	ms_env_set(&shell->env_list, "_", value, 1);
}

static int	ms_exec_directory_status(char *argv0, char *display_arg)
{
	if (ft_strcmp(argv0, ".") == 0)
	{
		ms_print_command_not_found(argv0);
		return (127);
	}
	write(STDERR_FILENO, display_arg, ft_strlen(display_arg));
	write(STDERR_FILENO, ": Is a directory\n", 17);
	return (126);
}

static int	ms_exec_precheck(char *argv0, char *display_arg, char *path)
{
	struct stat	file_info;

	if (stat(path, &file_info) == -1)
		return (ms_exec_error_code(display_arg, errno));
	if (S_ISDIR(file_info.st_mode))
		return (ms_exec_directory_status(argv0, display_arg));
	return (-1);
}

static int	ms_exec_external_command(t_shell *shell, char **argv)
{
	char	*path;
	char	**envp;
	char	*display_arg;
	int		used_path;
	int		err_no;
	int		status;
	int		err_no;

	path = ms_find_executable(shell, argv[0], &used_path);
	if (!path)
		return (ms_print_command_not_found(argv[0]), 127);
	display_arg = argv[0];
	if (used_path)
		display_arg = path;
	ms_update_underscore(shell, path);
	status = ms_exec_precheck(argv[0], display_arg, path);
	if (status >= 0)
		return (free(path), status);
	envp = ms_env_to_array(shell->env_list);
	execve(path, argv, envp);
	err_no = errno;
	ms_free_str_array(envp);
	status = ms_exec_error_code(display_arg, err_no);
	free(path);
	return (status);
}

static int	ms_dup_and_close(int from, int to)
{
	if (from != to)
	{
		if (dup2(from, to) < 0)
			return (-1);
		close(from);
	}
	return (0);
}

static void	ms_child_exit(t_shell *shell, t_command *cmd_list, int status)
{
	if (shell && shell->current_line)
	{
		free(shell->current_line);
		shell->current_line = NULL;
	}
	if (cmd_list)
		ms_free_command_list(cmd_list);
	if (shell)
		ms_free_shell(shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(status);
}

static void	ms_close_heredocs(t_command *cmd_list, t_command *current)
{
	t_command	*cl;
	t_redir		*r;

	cl = cmd_list;
	while (cl)
	{
		r = cl->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC && r->heredoc_fd >= 0 && cl != current)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		cl = cl->next;
	}
}

void	ms_execute_child(t_shell *shell, t_command *cmd_list, t_command *cmd,
		int in_fd, int out_fd)
{
	int	status;

	ms_setup_child_signals();
	if (ms_dup_and_close(in_fd, STDIN_FILENO) < 0)
		ms_child_exit(shell, cmd_list, 1);
	if (ms_dup_and_close(out_fd, STDOUT_FILENO) < 0)
		ms_child_exit(shell, cmd_list, 1);
	ms_close_heredocs(cmd_list, cmd);
	if (ms_apply_redirections(cmd->redirections) < 0)
		ms_child_exit(shell, cmd_list, 1);
	if (!cmd->argv || !cmd->argv[0])
		ms_child_exit(shell, cmd_list, 0);
	ms_update_underscore(shell, cmd->argv[0]);
	if (ms_is_builtin(cmd->argv[0]))
	{
		status = ms_run_builtin_child(shell, cmd->argv);
		ms_child_exit(shell, cmd_list, status);
	}
	status = ms_exec_external_command(shell, cmd->argv);
	ms_child_exit(shell, cmd_list, status);
}

// Juliyan to check pipe_fd
//         write()                    read()
//    pipe_fd[1]  ─────────▶  pipe_fd[0]

int	ms_fork_and_execute(t_shell *shell, t_command *cmd_list, t_command *cmd,
		int prev_read, int pipe_fd[2], pid_t *pids_to_free)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork\n");
		return (-1);
	}
	if (pid == 0)
	{
		ms_setup_child_signals();
		if (cmd->next)
			close(pipe_fd[0]);
		if (pids_to_free)
			free(pids_to_free);
		ms_execute_child(shell, cmd_list, cmd, prev_read,
			cmd->next ? pipe_fd[1] : STDOUT_FILENO);
	}
	return (pid);
}
