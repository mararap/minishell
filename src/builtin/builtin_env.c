#include "minishell.h"

/*
 * print_env_entry:
 *   Construct and print a single environment variable line in the
 *   format "NAME=value\n". We build one buffer per entry and write
 *   it in a single call for performance and clarity.
 *
 *   In minishell, env should list variables currently in the shell’s
 *   environment table. We assume `name` and `value` are valid C strings.
 */

static void	print_env_entry(char *name, char *value)
{
	size_t	name_len;
	size_t	val_len;
	size_t	total_len;
	char	*line;

	name_len = ft_strlen(name);
	val_len = ft_strlen(value);
	total_len = name_len + 1 + val_len + 1;
	line = ms_xmalloc(total_len);
	ft_memcpy(line, name, name_len);
	line[name_len] = '=';
	ft_memcpy(line + name_len + 1, value, val_len);
	line[total_len - 1] = '\n';
	write(STDOUT_FILENO, line, total_len);
	free(line);
}

/*
 * ms_builtin_env:
 *   Implementation of the `env` built‑in command for minishell.
 *
 *   According to the 42 minishell subject, `env` must:
 *     • not accept options or arguments — if anything is passed,
 *       print an error and return failure.
 *     • print all environment variables in the shell’s internal
 *       environment list (exported values) as "NAME=value" lines,
 *       one per line. :contentReference[oaicite:1]{index=1}
 *
 *   This function iterates the linked list of environment vars
 *   and prints only those with a non‑NULL value. Return codes
 *   follow common shell conventions: 0 on success, non‑zero on error.
 */

int	ms_builtin_env(t_shell *shell, char **argv)
{
	t_env_var	*iter;

	if (argv[1])
	{
		ft_putstr_fd("env: ", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	iter = shell->env_list;
	while (iter)
	{
		if (iter->value)
			print_env_entry(iter->name, iter->value);
		iter = iter->next;
	}
	return (EXIT_SUCCESS);
}
