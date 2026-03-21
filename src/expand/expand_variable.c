/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_variable.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:39:18 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/15 15:41:43 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ms_expand_variable(t_shell *shell, char *str, int *idx)
{
	int		start;
	char	*name;
	char	*value;

	if (str[*idx + 1] == '?')
	{
		*idx = *idx + 2;
		return (ft_itoa(shell->last_exit_status));
	}
	start = *idx + 1;
	while (str[start] && (ft_isalnum(str[start]) || str[start] == '_'))
		start++;
	if (start == *idx + 1)
	{
		*idx = *idx + 1;
		return (ft_strdup("$"));
	}
	name = ft_substr(str, *idx + 1, start - (*idx + 1));
	value = ms_env_get_value(shell->env_list, name);
	free(name);
	*idx = start;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}
