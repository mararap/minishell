#include "minishell.h"

int	ms_builtin_env(t_shell *shell, char **argv)
{
	t_env_var	*iter;

	if (argv[1])
	{
		write(STDERR_FILENO, SHELL_NAME ": env: no arguments supported\n", 47);
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

static int	ms_parse_export_pair(char *arg, char **name, char **value)
{
	char	*sep;

	sep = ft_strchr(arg, '=');
	if (!sep)
	{
		*name = ft_strdup(arg);
		*value = NULL;
		return (0);
	}
	*name = ft_substr(arg, 0, sep - arg);
	*value = ft_strdup(sep + 1);
	return (0);
}

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int		i;
	char	*name;
	char	*value;

	if (!argv[1])
		return (ms_builtin_env(shell, argv));
	i = 1;
	while (argv[i])
	{
		name = NULL;
		value = NULL;
		ms_parse_export_pair(argv[i], &name, &value);
		if (name && name[0] != '\0')
			ms_env_set(&shell->env_list, name, value ? value : "");
		free(name);
		if (value)
			free(value);
		i++;
	}
	return (0);
}

int	ms_builtin_unset(t_shell *shell, char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		ms_env_unset(&shell->env_list, argv[i]);
		i++;
	}
	return (0);
}
