#include "minishell.h"

static char	**ms_env_to_array_full(t_env_var *env_list)
{
	char	**arr;
	int		len;
	int		i;
	char	*entry;

	len = 0;
	while (env_list)
	{
		len++;
		env_list = env_list->next;
	}
	arr = (char **)ms_xmalloc(sizeof(char *) * (len + 1));
	i = 0;
	env_list = env_list ? env_list : NULL;
	while (env_list && i < len)
	{
		entry = ms_str_join_three(env_list->name, "=", env_list->value);
		arr[i] = entry;
		env_list = env_list->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

static char	*ms_find_executable(t_shell *shell, char *cmd)
{
	char	*path_env;
	char	**paths;
	char	*candidate;
	int		i;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = ms_env_get_value(shell->env_list, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		candidate = ms_str_join_three(paths[i], "/", cmd);
		if (access(candidate, X_OK) == 0)
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

static void	ms_exec_external_command(t_shell *shell, char **argv)
{
	char	*path;
	char	**envp;

	path = ms_find_executable(shell, argv[0]);
	if (!path)
	{
		ms_print_command_not_found(argv[0]);
		exit(127);
	}
	envp = ms_env_to_array_full(shell->env_list);
	execve(path, argv, envp);
	ms_perror(path);
	ms_free_str_array(envp);
	free(path);
	exit(126);
}

static void	ms_dup_and_close(int from, int to)
{
	if (from != to)
	{
		if (dup2(from, to) < 0)
		{
			ms_perror("dup2");
			exit(1);
		}
		close(from);
	}
}

static void	ms_execute_child(t_shell *shell, t_command *cmd,
			int in_fd, int out_fd)
{
	ms_dup_and_close(in_fd, STDIN_FILENO);
	ms_dup_and_close(out_fd, STDOUT_FILENO);
	if (ms_apply_redirections(cmd->redirections, shell) < 0)
		exit(1);
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (ms_is_builtin(cmd->argv[0]))
		exit(ms_run_builtin_child(shell, cmd->argv));
	ms_exec_external_command(shell, cmd->argv);
	exit(127);
}

int	ms_fork_and_execute(t_shell *shell, t_command *cmd,
	int prev_read, int pipe_fd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		ms_perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		ms_setup_child_signals();
		if (cmd->next)
			close(pipe_fd[0]);
		ms_execute_child(shell, cmd, prev_read,
			cmd->next ? pipe_fd[1] : STDOUT_FILENO);
	}
	return (pid);
}


