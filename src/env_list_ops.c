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

static t_env_var	*ms_env_make_node(char *name, char *value, int exported)
{
	t_env_var	*node;

	node = (t_env_var *)ms_xmalloc(sizeof(t_env_var));
	node->name = ms_strdup_safe(name);
	node->value = ms_strdup_safe(value);
	node->exported = exported;
	node->next = NULL;
	return (node);
}

void	ms_env_free_list(t_env_var **env_list)
{
	t_env_var	*next;

	if (!env_list)
		return ;
	while (*env_list)
	{
		next = (*env_list)->next;
		free((*env_list)->name);
		free((*env_list)->value);
		free(*env_list);
		*env_list = next;
	}
}

int	ms_env_set(t_env_var **env_list, char *name, char *value, int exported)
{
	t_env_var	*var;
	t_env_var	*tail;

	if (!name || !ms_env_is_valid_name(name))
		return (1);
	var = ms_env_find_var(*env_list, name);
	if (var)
	{
		free(var->value);
		var->value = ms_strdup_safe(value);
		var->exported = exported;
		return (0);
	}
	var = ms_env_make_node(name, value, exported);
	tail = *env_list;
	while (tail && tail->next)
		tail = tail->next;
	if (!tail)
		*env_list = var;
	else
		tail->next = var;
	return (0);
}