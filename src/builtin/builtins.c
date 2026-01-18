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

int	ms_run_builtin_parent(t_shell *shell, t_command *cmd)
{
	int	status;

	status = ms_run_builtin_child(shell, cmd->argv);
	return (status);
}
