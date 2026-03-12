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

static int	ms_count_env_vars(t_env_var *env_list)
{
	int			count;
	t_env_var	*current;

	count = 0;
	current = env_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

static void	ms_sort_env_array(t_env_var **arr, int size)
{
	int			i;
	int			j;
	t_env_var	*tmp;

	i = 0;
	while (i < size - 1)
	{
		j = i + 1;
		while (j < size)  //sort env array alphabetically
		{
			if (ft_strcmp(arr[i]->name, arr[j]->name) > 0)
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	ms_print_export_format(t_env_var *env_list)
{
	t_env_var	**arr;
	t_env_var	*current;
	int			count;
	int			i;

	count = ms_count_env_vars(env_list);
	if (count == 0)
		return ;
	
	arr = malloc(sizeof(t_env_var *) * count); //array of pointers for sorting
	if (!arr)
		return ;
	current = env_list; //start to fill array
	i = 0;
	while (current)
	{
		arr[i++] = current;
		current = current->next;
	}
	ms_sort_env_array(arr, count); //sort alphabetically
	i = 0;
	while (i < count)
	{
		if (ft_strcmp(arr[i]->name, "_") == 0)
		{
			i++;
			continue;
		}
		if (arr[i]->exported == 0)
		{
			i++;
			continue;
		}
		ft_putstr_fd("export ", 1);
		ft_putstr_fd(arr[i]->name, 1);
		if (arr[i]->value && arr[i]->value[0] != '\0') //only if value exists
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(arr[i]->value, 1);
			ft_putstr_fd("\"", 1);
		}
		ft_putstr_fd("\n", 1);
		i++;
	}
	free(arr);
}

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int		i;
	char	*name;
	char	*value;
	char	*old_val;
	int		exit_code;

	if (!argv[1])
	{
		ms_print_export_format(shell->env_list);
		return (0);
	}
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
