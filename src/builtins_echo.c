#include "minishell.h"

static int	ms_is_valid_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	i = 1;
	if (arg[1] == '\0')
		return (0);
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	ms_builtin_echo(char **argv)
{
	int	i;
	int	print_newline;

	i = 1;
	print_newline = 1;
	if (argv[1] && ms_is_valid_n_flag(argv[1]))
	{
		print_newline = 0;
		while (argv[i] && ms_is_valid_n_flag(argv[i]))
			i++;
	}
	while (argv[i])
	{
		write(STDOUT_FILENO, argv[i], ft_strlen(argv[i]));
		if (argv[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (print_newline)
		write(STDOUT_FILENO, "\n", 1);
	return (0);
}
