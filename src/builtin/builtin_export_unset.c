#include "minishell.h"

static int	ms_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_') //check if first char is letter or '_'
		return (0);
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	ms_parse_export_pair(char *arg, char **name, char **value)
{
	char	*sep;

	sep = ft_strchr(arg, '=');
	if (!sep)
	{
		*name = ft_strdup(arg);
		*value = NULL;
		if (!*name)
			return (1);
		return (0);
	}
	*name = ft_substr(arg, 0, sep - arg);
	*value = ft_strdup(sep + 1);
	if (!*name || !*value)
	{
		free(*name);
		free(*value);
		return (1);
	}
	return (0);
}

static void	ms_export_error(char *arg)
{
	ft_putstr_fd("juma[n]she: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int		i;
	char	*name;
	char	*value;
	char	*old_val;
	int		exit_code;

	if (!argv[1])
		return (ms_builtin_env(shell, argv));
	i = 1;
	exit_code = 0;
	while (argv[i])
	{
		name = NULL;
		value = NULL;
		if (ms_parse_export_pair(argv[i], &name, &value) == 0)
		{
			if (name && !ms_is_valid_identifier(name))
			{
				ms_export_error(argv[i]);
				exit_code = 1;
			}
			else if (name && name[0] != '\0')
			{
				if (value != NULL)
				{
					// export with assignment
					ms_env_set(&shell->env_list, name, value, 1);
				}
				else
				{
					// just export name,
					// get old value if any
					old_val = ms_env_get_value(shell->env_list, name);
					ms_env_set(&shell->env_list, name,
							old_val ? old_val : "", 1);
				}
			}
		}
		free(name);
		free(value);
		i++;
	}
	return (exit_code);
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
