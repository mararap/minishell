/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ifs_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 16:28:23 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/18 18:58:37 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define MS_MASK_SPACE 0x1F
#define MS_MASK_TAB 0x1E
#define MS_MASK_NL 0x1D

void	ms_unmask_ifs(char *s)
{
	if (!s)
		return ;
	while (*s)
	{
		if (*s == MS_MASK_SPACE)
			*s = ' ';
		else if (*s == MS_MASK_TAB)
			*s = '\t';
		else if (*s == MS_MASK_NL)
			*s = '\n';
		s++;
	}
}

static int	ms_is_ifs_delim(char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
		return (1);
	return (0);
}

static size_t	ms_count_ifs_fields(const char *s)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] && ms_is_ifs_delim(s[i]))
			i++;
		if (!s[i])
			break ;
		count++;
		while (s[i] && !ms_is_ifs_delim(s[i]))
			i++;
	}
	return (count);
}

static char	*ms_next_field(const char *s, size_t *i)
{
	size_t	start;
	char	*field;

	while (s[*i] && ms_is_ifs_delim(s[*i]))
		(*i)++;
	if (!s[*i])
		return (NULL);
	start = *i;
	while (s[*i] && !ms_is_ifs_delim(s[*i]))
		(*i)++;
	field = ft_substr(s, start, *i - start);
	if (!field)
	{
		write(STDERR_FILENO, SHELL_NAME
			": malloc failed\n", ft_strlen(SHELL_NAME) + 16);
		exit(1);
	}
	ms_unmask_ifs(field);
	return (field);
}

char	**ms_split_ifs_fields(const char *s)
{
	char	**out;
	size_t	fields;
	size_t	i;
	size_t	j;

	fields = ms_count_ifs_fields(s);
	i = 0;
	j = 0;
	out = (char **)ms_xmalloc(sizeof(char *) * (fields + 1));
	while (s[i])
	{
		out[j] = ms_next_field(s, &i);
		if (!out[j])
			break ;
		j++;
	}
	out[j] = NULL;
	return (out);
}
