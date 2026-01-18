#include "minishell.h"

static int	ms_is_numeric(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	ms_builtin_exit(t_shell *shell, char **argv)
{
	long	code;

	(void)shell;
	write(STDOUT_FILENO, "exit\n", 5);
	if (!argv[1])
		exit(0);
	if (!ms_is_numeric(argv[1]))
	{
		write(STDERR_FILENO, SHELL_NAME ": exit: numeric argument required\n",
			51);
		exit(2);
	}
	if (argv[2])
	{
		write(STDERR_FILENO, SHELL_NAME ": exit: too many arguments\n", 40);
		return (1);
	}
	code = atol(argv[1]);
	exit((unsigned char)code);
}
