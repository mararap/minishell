#include "minishell.h"

int	ms_is_builtin(char *cmd_name)
{
	if (!cmd_name)
		return (0);
	if (ft_strncmp(cmd_name, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "env", 4) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "exit", 5) == 0)
		return (1);
	return (0);
}

int	ms_builtin_needs_parent(char *cmd_name)
{
	if (!cmd_name)
		return (0);
	if (ft_strncmp(cmd_name, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd_name, "exit", 5) == 0)
		return (1);
	return (0);
}

int	ms_run_builtin_child(t_shell *shell, char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (ft_strncmp(argv[0], "echo", 5) == 0)
		return (ms_builtin_echo(argv));
	if (ft_strncmp(argv[0], "cd", 3) == 0)
		return (ms_builtin_cd(shell, argv));
	if (ft_strncmp(argv[0], "pwd", 4) == 0)
		return (ms_builtin_pwd(shell));
	if (ft_strncmp(argv[0], "env", 4) == 0)
		return (ms_builtin_env(shell, argv));
	if (ft_strncmp(argv[0], "export", 7) == 0)
		return (ms_builtin_export(shell, argv));
	if (ft_strncmp(argv[0], "unset", 6) == 0)
		return (ms_builtin_unset(shell, argv));
	if (ft_strncmp(argv[0], "exit", 5) == 0)
		return (ms_builtin_exit(shell, argv));
	return (0);
}

static int	ms_dup_stdio(int *save_in, int *save_out, int *save_err)
{
	*save_in = dup(STDIN_FILENO);
	*save_out = dup(STDOUT_FILENO);
	*save_err = dup(STDERR_FILENO);
	if (*save_in < 0 || *save_out < 0 || *save_err < 0)
	{
		if (*save_in >= 0)
			close(*save_in);
		if (*save_out >= 0)
			close(*save_out);
		if (*save_err >= 0)
			close(*save_err);
		return (-1);
	}
	return (0);
}

static void	ms_restore_stdio(int save_in, int save_out, int save_err)
{
	if (save_in >= 0)
	{
		if (save_in != STDIN_FILENO)
			dup2(save_in, STDIN_FILENO);
		close(save_in);
	}
	if (save_out >= 0)
	{
		if (save_out != STDOUT_FILENO)
			dup2(save_out, STDOUT_FILENO);
		close(save_out);
	}
	if (save_err >= 0)
	{
		if (save_err != STDERR_FILENO)
			dup2(save_err, STDERR_FILENO);
		close(save_err);
	}
}

/*static int	ms_only_output_redirs(t_redir *redirs)
{
	while (redirs)
	{
		if (redirs->type == REDIR_IN || redirs->type == REDIR_HEREDOC)
			return (0);
		redirs = redirs->next;
	}
	return (1);
}*/

int	ms_run_builtin_parent(t_shell *shell, t_command *cmd)
{
	int	save_in;
	int	save_out;
	int	save_err;
	int	status;

	if (!cmd->redirections)
		return (ms_run_builtin_child(shell, cmd->argv));
	/*if (ms_only_output_redirs(cmd->redirections)
		&& (ft_strncmp(cmd->argv[0], "cd", 3) == 0
		|| ft_strncmp(cmd->argv[0], "unset", 6) == 0))
		return (ms_run_builtin_child(shell, cmd->argv));*/
	if (ms_dup_stdio(&save_in, &save_out, &save_err) < 0)
		return (perror("dup"), 1);
	status = 0;
	if (cmd->redirections && ms_apply_redirections(cmd->redirections) < 0)
		status = 1; // redirection failed => builtin not executed
	else
		status = ms_run_builtin_child(shell, cmd->argv);
	ms_restore_stdio(save_in, save_out, save_err);
	return (status);
}
