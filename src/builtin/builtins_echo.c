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
	int	first;

	i = 1;
	print_newline = 1;
	first = 1;

	// Skip all valid -n flags
	while (argv[i] && ms_is_valid_n_flag(argv[i]))
	{
		print_newline = 0;
		i++;
	}

	// Print remaining arguments
	while (argv[i])
	{
		if (!first)
			write(STDOUT_FILENO, " ", 1);
		write(STDOUT_FILENO, argv[i], ft_strlen(argv[i]));
		first = 0;
		i++;
	}

	if (print_newline)
		write(STDOUT_FILENO, "\n", 1);

	return (0);
}
