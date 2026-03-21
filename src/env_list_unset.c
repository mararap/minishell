/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list_array.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_env_unset(t_env_var **env_list, char *name)
{
	t_env_var	*iter;
	t_env_var	*prev;

	if (!name || !ms_env_is_valid_name(name))
		return (1);
	iter = *env_list;
	prev = NULL;
	while (iter)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			if (!prev)
				*env_list = iter->next;
			else
				prev->next = iter->next;
			free(iter->name);
			free(iter->value);
			free(iter);
			return (0);
		}
		prev = iter;
		iter = iter->next;
	}
	return (0);
}