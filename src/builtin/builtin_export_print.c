/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:50 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/20 11:34:15 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static void	ms_print_export_entry(t_env_var *var)
{
	ft_putstr_fd("export ", STDOUT_FILENO);
	ft_putstr_fd(var->name, STDOUT_FILENO);
	if (var->value != NULL)
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(var->value, STDOUT_FILENO);
		ft_putstr_fd("\"", STDOUT_FILENO);
	}
	ft_putstr_fd("\n", STDOUT_FILENO);
}

void	ms_print_export_format(t_env_var *env_list)
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
		if (ft_strcmp(arr[i]->name, "_") != 0 && arr[i]->exported != 0)
			ms_print_export_entry(arr[i]);
		i++;
	}
	free(arr);
}
