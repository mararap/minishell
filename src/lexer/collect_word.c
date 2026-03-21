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

static int	ms_join_piece(char **buf, char *tmp)
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

static char	*ms_collect_plain_word(char *str, int *idx)
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

static char	*ms_collect_piece(t_word_ctx *wctx)
{
	if (wctx->str[*wctx->idx] == '\'')
	{
		*wctx->was_quoted = 1;
		return (ms_collect_single_quotes(wctx->str, wctx->idx));
	}
	if (wctx->str[*wctx->idx] == '"')
	{
		*wctx->was_quoted = 1;
		return (ms_collect_double_quotes(wctx->shell, wctx->str, wctx->idx,
				wctx->allow_expansion));
	}
	if (wctx->str[*wctx->idx] == '$' && wctx->str[*wctx->idx + 1] == '"')
	{
		*wctx->was_quoted = 1;
		return (ms_collect_locale_quotes(wctx->shell, wctx->str, wctx->idx,
				wctx->allow_expansion));
	}
	if (wctx->str[*wctx->idx] == '$' && wctx->allow_expansion)
		return (ms_expand_variable(wctx->shell, wctx->str, wctx->idx));
	if (wctx->str[*wctx->idx] == '$')
		return (ms_collect_literal_dollar(wctx->str, wctx->idx));
	return (ms_collect_plain_word(wctx->str, wctx->idx));
}

char	*ms_collect_word(t_word_ctx *wctx)
{
 	char	*buf;
 	char	*tmp;
 
 	buf = ft_strdup("");
	*wctx->was_quoted = 0;
	while (wctx->str[*wctx->idx] && wctx->str[*wctx->idx] != ' '
		&& wctx->str[*wctx->idx] != '\t' && wctx->str[*wctx->idx] != '|'
		&& wctx->str[*wctx->idx] != '<' && wctx->str[*wctx->idx] != '>')
 	{
		tmp = ms_collect_piece(wctx);
 		if (!tmp || !ms_join_piece(&buf, tmp))
		{
			free(buf);
			return (NULL);
		}
 	}
 	return (buf);
}
