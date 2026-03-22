/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list_get.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env_var	*ms_env_find_var(t_env_var *env_list, char *name)
{
	size_t	len;

	if (!name)
		return (NULL);
	len = ft_strlen(name);
	while (env_list)
	{
		if (ft_strlen(env_list->name) == len && ft_strncmp(env_list->name, name,
				len) == 0)
			return (env_list);
		env_list = env_list->next;
	}
	return (NULL);
}

char	*ms_env_get_value(t_env_var *env_list, char *name)
{
	t_env_var	*var;

	var = ms_env_find_var(env_list, name);
	if (!var)
		return (NULL);
	return (var->value);
}
