#include "minishell.h"

/* static char	**ms_env_to_array_full(t_env_var *env_list)
{
	char		**arr;
	int			len;
	int			i;
	char		*entry;
	t_env_var	*head;

	head = env_list;
	len = 0;
	while (env_list)
	{
		len++;
		env_list = env_list->next;
	}
	arr = (char **)ms_xmalloc(sizeof(char *) * (len + 1));
	i = 0;
	env_list = head;
	while (env_list && i < len)
	{
		entry = ms_str_join_three(env_list->name, "=", env_list->value);
		arr[i] = entry;
		env_list = env_list->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
} */

static char	*ms_find_executable(t_shell *shell, char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*candidate;
	int		i;
	char	*cwd;

	if (!cmd || cmd[0] == '\0')
		return (NULL);

    // If the command contains '/', treat it as a relative or absolute path
    if (ft_strchr(cmd, '/'))
		return ft_strdup(cmd);
	
	// Get PATH from environment
	path_env = ms_env_get_value(shell->env_list, "PATH");
	
	// If PATH is unset, bash still checks CWD - find file there for proper 126 error
	if (!path_env || path_env[0] == '\0')
	{
		cwd = getcwd(NULL, 0);
		if (!cwd)
			return (NULL);
		candidate = ms_str_join_three(cwd, "/", cmd);
		free(cwd);
		return (candidate);
	}
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		if (paths[i][0] == '\0')
			candidate = ms_str_join_three(".", "/", cmd);
		else
			candidate = ms_str_join_three(paths[i], "/", cmd);
		if (access(candidate, X_OK) == 0)
		{
			ms_free_str_array(paths);
			return (candidate);
		}
		if (access(candidate, F_OK) == 0)
		{
			ms_free_str_array(paths);
			return (candidate);
		}
		free(candidate);
		i++;
	}
	ms_free_str_array(paths);
	return (NULL);
}

static int	ms_exec_error_code(char *arg, int err_no)
{
	int	exit_code;

	exit_code = 127;
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": ", 2);
	if (err_no == ENOENT)
		write(STDERR_FILENO, "No such file or directory\n", 26);
	else if (err_no == EISDIR || err_no == ENOEXEC)
	{
		if (err_no == EISDIR)
			write(STDERR_FILENO, "Is a directory\n", 15);
		else
			write(STDERR_FILENO, "Exec format error\n", 18);
		exit_code = 126;
	}
	else if (err_no == EACCES)
	{
		write(STDERR_FILENO, "Permission denied\n", 18);
		exit_code = 126;
	}
	else
	{
		write(STDERR_FILENO, strerror(err_no), ft_strlen(strerror(err_no)));
		write(STDERR_FILENO, "\n", 1);
	}
	return (exit_code);
}

static void	ms_update_underscore(t_shell *shell, char *value)
{
	if (!shell || !value || value[0] == '\0')
		return ;
	ms_env_set(&shell->env_list, "_", value, 1);
}

static int	ms_exec_external_command(t_shell *shell, char **argv)
{
	char		*path;
	char		**envp;
	struct stat	file_info;
	int			err_no;

	path = ms_find_executable(shell, argv[0]);
	if (!path)
	{
		ms_print_command_not_found(argv[0]);
		return (127);
	}
	ms_update_underscore(shell, path);
	if (stat(path, &file_info) == -1)
	{
		err_no = errno;
		free(path);
		return (ms_exec_error_code(argv[0], err_no));
	}
	if (S_ISDIR(file_info.st_mode))
	{
		if (ft_strcmp(argv[0], ".") == 0)
		{
			ms_print_command_not_found(argv[0]);
			free(path);
			return (127);
		}
		write (2, argv[0], ft_strlen(argv[0]));
		write (2, ": Is a directory\n", 17);
		free(path);
		return (126);
	}
	envp = ms_env_to_array(shell->env_list);
	execve(path, argv, envp);
	err_no = errno;
	ms_free_str_array(envp);
	free(path);
	return (ms_exec_error_code(argv[0], err_no));
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
			if (r->type == REDIR_HEREDOC && r->heredoc_fd >= 0
				&& cl != current)
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

//Juliyan to check pipe_fd
//         write()                    read()
//    pipe_fd[1]  ─────────▶  pipe_fd[0]

int	ms_fork_and_execute(t_shell *shell, t_command *cmd_list, t_command *cmd,
	int prev_read, int pipe_fd[2])
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
		ms_execute_child(shell, cmd_list, cmd, prev_read,
			cmd->next ? pipe_fd[1] : STDOUT_FILENO);
	}
	return (pid);
}


