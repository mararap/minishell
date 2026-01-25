#include "minishell.h"

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

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int		i;
	char	*name;
	char	*value;
	char	*old_val;

	if (!argv[1])
		return (ms_builtin_env(shell, argv));
	i = 1;
while (argv[i])
{
    name = NULL;
    value = NULL;
    if (ms_parse_export_pair(argv[i], &name, &value) == 0)
    {
        if (name != NULL && name[0] != '\0')
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
                ms_env_set(&shell->env_list,
                           name,
                           old_val ? old_val : "",
                           1);
            }
        }
    }
    free(name);
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
