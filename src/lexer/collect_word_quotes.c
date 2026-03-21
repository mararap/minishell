/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collect_word_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 14:01:13 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 14:08:47 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_mask_ifs(char *s)
{
	if (!s)
		return ;
	while (*s)
	{
		if (*s == ' ')
			*s = MS_MASK_SPACE;
		else if (*s == '\t')
			*s = MS_MASK_TAB;
		else if (*s == '\n')
			*s = MS_MASK_NL;
		s++;
	}
}

char	*ms_collect_single_quotes(char *str, int *idx)
{
	int		start;
	char	*tmp;

	(*idx)++;
	start = *idx;
	while (str[*idx] && str[*idx] != '\'')
		(*idx)++;
	tmp = ft_substr(str, start, *idx - start);
	if (!tmp)
		return (NULL);
	ms_mask_ifs(tmp);
	if (str[*idx] == '\'')
		(*idx)++;
	return (tmp);
}

static char	*ms_collect_dq_chunk(t_shell *shell, char *str, int *idx,
	int allow_expansion)
{
	int	start;

	if (str[*idx] == '$' && allow_expansion)
		return (ms_expand_variable(shell, str, idx));
	start = *idx;
	while (str[*idx] && str[*idx] != '"'
		&& !(str[*idx] == '$' && allow_expansion))
		(*idx)++;
	return (ft_substr(str, start, *idx - start));
}

char	*ms_collect_double_quotes(t_shell *shell, char *str, int *idx,
	int allow_expansion)
{
	char	*buf;
	char	*tmp;
	char	*old_buf;

	(*idx)++;
	buf = ft_strdup("");
	if (!buf)
		return (NULL);
	while (str[*idx] && str[*idx] != '"')
	{
		tmp = ms_collect_dq_chunk(shell, str, idx, allow_expansion);
		if (!tmp)
			return (free(buf), NULL);
		ms_mask_ifs (tmp);
		old_buf = buf;
		buf = ft_strjoin(buf, tmp);
		free(old_buf);
		free(tmp);
		if (!buf)
			return (NULL);
	}
	if (str[*idx] == '"')
		(*idx)++;
	return (buf);
}

char	*ms_collect_locale_quotes(t_shell *shell, char *str, int *idx,
	int allow_expansion)
{
	(*idx)++;
	return (ms_collect_double_quotes(shell, str, idx, allow_expansion));
}
