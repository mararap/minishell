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

static int	ms_env_exported_count(t_env_var *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		if (env_list->exported == 1 && env_list->value)
			count++;
		env_list = env_list->next;
	}
	return (count);
}

static char	*ms_env_make_entry(t_env_var *var)
{
	size_t	name_len;
	size_t	val_len;
	char	*str;

	name_len = ft_strlen(var->name);
	val_len = ft_strlen(var->value);
	str = ms_xmalloc(sizeof(char) * (name_len + val_len + 2));
	ft_strlcpy(str, var->name, name_len + 1);
	ft_strlcat(str, "=", name_len + val_len + 2);
	ft_strlcat(str, var->value, name_len + val_len + 2);
	return (str);
}

char	**ms_env_to_array(t_env_var *env_list)
{
	char	**envp;
	int		count;
	int		i;

	count = ms_env_exported_count(env_list);
	envp = ms_xmalloc(sizeof(char *) * (count + 1));
	i = 0;
	while (env_list)
	{
		if (env_list->exported == 1 && env_list->value)
			envp[i++] = ms_env_make_entry(env_list);
		env_list = env_list->next;
	}
	envp[i] = NULL;
	return (envp);
}
