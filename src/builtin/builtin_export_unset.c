/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_unset.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:50 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/15 15:24:52 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_')
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
		return (!*name);
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
	ft_putstr_fd("juma[n]she: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
}

static int	ms_count_env_vars(t_env_var *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		count++;
		env_list = env_list->next;
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
		while (j < size)
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

static t_env_var	**ms_export_sorted_env(t_env_var *env_list, int *count)
{
	t_env_var	**arr;
	int			i;

	*count = ms_count_env_vars(env_list);
	if (*count == 0)
		return (NULL);
	arr = malloc(sizeof(t_env_var *) * (*count));
	if (!arr)
		return (NULL);
	i = 0;
	while (env_list)
	{
		arr[i++] = env_list;
		env_list = env_list->next;
	}
	ms_sort_env_array(arr, *count);
	return (arr);
}
static int	ms_skip_export_entry(t_env_var *var)
{
	if (ft_strcmp(var->name, "_") == 0)
		return (1);
	if (var->exported == 0)
		return (1);
	return (0);
}
static void	ms_print_export_entry(t_env_var *var)
{
	ft_putstr_fd("export ", STDOUT_FILENO);
	ft_putstr_fd(var->name, STDOUT_FILENO);
	if (var->value && var->value[0] != '\0')
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(var->value, STDOUT_FILENO);
		ft_putstr_fd("\"", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
}
static void	ms_print_export_format(t_env_var *env_list)
{
	t_env_var	**arr;
	int			count;
	int			i;

	arr = ms_export_sorted_env(env_list, &count);
	if (!arr)
		return ;
	i = 0;
	while (i < count)
	{
		if (!ms_skip_export_entry(arr[i]))
			ms_print_export_entry(arr[i]);
		i++;
	}
	free(arr);
}
static int	ms_export_name_only(t_shell *shell, char *name)
{
	char	*old_val;

	old_val = ms_env_get_value(shell->env_list, name);
	return (ms_env_set(&shell->env_list, name, old_val ? old_val : "", 1));
}
static int	ms_export_one_arg(t_shell *shell, char *arg)
{
	char	*name;
	char	*value;
	int		status;

	name = NULL;
	value = NULL;
	status = ms_parse_export_pair(arg, &name, &value);
	if (status == 0 && (!name || !ms_is_valid_identifier(name)))
		status = (ms_export_error(arg), 1);
	else if (status == 0 && value != NULL)
		status = ms_env_set(&shell->env_list, name, value, 1);
	else if (status == 0 && name && name[0] != '\0')
		status = ms_export_name_only(shell, name);
	free(name);
	free(value);
	return (status != 0);
}

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int	i;
	int	exit_code;

	if (!argv[1])
	{
		ms_print_export_format(shell->env_list);
		return (0);
	}
	i = 1;
	exit_code = 0;
	while (argv[i])
	{
		if (ms_export_one_arg(shell, argv[i]))
			exit_code = 1;
		i++;
	}
	return (exit_code);
}


