/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collect_word.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 19:51:20 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/15 19:54:05 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_join_piece(char **buf, char *tmp)
{
	char	*new_buf;

	new_buf = ft_strjoin(*buf, tmp);
	free(*buf);
	free(tmp);
	if (!new_buf)
		return (0);
	*buf = new_buf;
	return (1);
}

char	*ms_collect_plain_word(char *str, int *idx)
{
	int	start;

	start = *idx;
	while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
		&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>'
		&& str[*idx] != '\'' && str[*idx] != '"' && str[*idx] != '$')
		(*idx)++;
	return (ft_substr(str, start, *idx - start));
}

static char	*ms_collect_literal_dollar(char *str, int *idx)
{
	int	start;

	start = *idx;
	(*idx)++;
	while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
		&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>'
		&& str[*idx] != '\'' && str[*idx] != '"')
		(*idx)++;
	return (ft_substr(str, start, *idx - start));
}

static char	*ms_collect_piece(t_shell *shell, char *str, int *idx,
		int *was_quoted, int allow_expansion)
{
	if (str[*idx] == '\'')
		return (*was_quoted = 1, ms_collect_single_quotes(str, idx));
	if (str[*idx] == '"')
		return (*was_quoted = 1, ms_collect_double_quotes(shell, str, idx,
				allow_expansion));
	if (str[*idx] == '$' && str[*idx + 1] == '"')
		return (*was_quoted = 1, ms_collect_locale_quotes(shell, str, idx,
				allow_expansion));
	if (str[*idx] == '$' && allow_expansion)
		return (ms_expand_variable(shell, str, idx));
	if (str[*idx] == '$')
		return (ms_collect_literal_dollar(str, idx));
	return (ms_collect_plain_word(str, idx));
}

char	*ms_collect_word(t_shell *shell, char *str, int *idx, int *was_quoted,
		int allow_expansion)
{
	char	*buf;
	char	*tmp;

	buf = ft_strdup("");
	*was_quoted = 0; // Initialize quote flag
	while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
		&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>')
	{
		tmp = ms_collect_piece(shell, str, idx, was_quoted, allow_expansion);
		if (!tmp || !ms_join_piece(&buf, tmp))
			return (free(buf), NULL);
	}
	return (buf);
}
