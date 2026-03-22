/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_env_is_valid_name(const char *str)
{
	if (!str)
		return (0);
	if (!ft_isalpha(*str) && *str != '_')
		return (0);
	str++;
	while (*str)
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (0);
		str++;
	}
	return (1);
}

static t_env_var	*ms_env_new_node(char *name, char *value, int exported)
{
	t_env_var	*node;

	if (!name || !ms_env_is_valid_name(name))
		return (NULL);
	node = ms_xmalloc(sizeof(t_env_var));
	node->name = ms_strdup_safe(name);
	node->value = NULL;
	if (value)
		node->value = ms_strdup_safe(value);
	node->exported = exported;
	node->next = NULL;
	return (node);
}

static int	ms_env_split_entry(char *entry, char **name, char **value)
{
	char	*sep;

	sep = ft_strchr(entry, '=');
	if (!sep)
	{
		*name = ft_strdup(entry);
		*value = NULL;
		return (!*name);
	}
	*name = ft_substr(entry, 0, sep - entry);
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

static void	ms_env_append_node(t_env_var **head, t_env_var **tail,
		t_env_var *node)
{
	if (!node)
		return ;
	if (!*head)
		*head = node;
	else
		(*tail)->next = node;
	*tail = node;
}

t_env_var	*ms_env_from_environ(char **envp)
{
	int			i;
	char		*name;
	char		*value;
	t_env_var	*head;
	t_env_var	*tail;

	i = 0;
	head = NULL;
	tail = NULL;
	while (envp && envp[i])
	{
		name = NULL;
		value = NULL;
		if (!ms_env_split_entry(envp[i], &name, &value)
			&& ms_env_is_valid_name(name))
			ms_env_append_node(&head, &tail,
				ms_env_new_node(name, value, 1));
		free(name);
		free(value);
		i++;
	}
	return (head);
}
