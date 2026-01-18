#include "minishell.h"

int	ms_builtin_env(t_shell *shell, char **argv)
{
	t_env_var	*iter;

	if (argv[1])
	{
		write(STDERR_FILENO, SHELL_NAME ": env: no arguments supported\n",
			sizeof(SHELL_NAME ": env: no arguments supported\n") - 1);
		return (1);
	}
	iter = shell->env_list;
	while (iter)
	{
		if (iter->value)
		{
			write(STDOUT_FILENO, iter->name, ft_strlen(iter->name));
			write(STDOUT_FILENO, "=", 1);
			write(STDOUT_FILENO, iter->value, ft_strlen(iter->value));
			write(STDOUT_FILENO, "\n", 1);
		}
		iter = iter->next;
	}
	return (0);
}