/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	*ms_xmalloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		write(STDERR_FILENO, SHELL_NAME ": malloc failed\n",
			ft_strlen(SHELL_NAME) + 16);
		exit(1);
	}
	return (ptr);
}

char	*ms_strdup_safe(const char *src)
{
	if (!src)
		return (NULL);
	return (ft_strdup(src));
}

size_t	ms_str_arr_len(char **arr)
{
	size_t	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

void	ms_free_str_array(char **arr)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

char	*ms_str_join_three(char *a, char *b, char *c)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(a, b);
	if (!tmp)
		return (NULL);
	res = ft_strjoin(tmp, c);
	free(tmp);
	return (res);
}
