/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
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
		*name = NULL;
		*value = NULL;
		return (1);
	}
	return (0);
}

static void	ms_export_error(char *arg)
{
	ft_putstr_fd(SHELL_NAME, STDERR_FILENO);
	ft_putstr_fd(": export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
}

static int	ms_export_name_only(t_shell *shell, char *name)
{
	char	*old_val;

	old_val = ms_env_get_value(shell->env_list, name);
	if (!old_val)
		old_val = "";
	return (ms_env_set(&shell->env_list, name, old_val, 1));
}

int	ms_export_one_arg(t_shell *shell, char *arg)
{
	char	*name;
	char	*value;
	int		status;

	name = NULL;
	value = NULL;
	status = ms_parse_export_pair(arg, &name, &value);
	if (status == 0 && (!name || !ms_is_valid_identifier(name)))
	{
		ms_export_error(arg);
		status = 1;
	}
	else if (status == 0 && value != NULL)
		status = ms_env_set(&shell->env_list, name, value, 1);
	else if (status == 0 && name && name[0] != '\0')
		status = ms_export_name_only(shell, name);
	free(name);
	free(value);
	return (status != 0);
}
