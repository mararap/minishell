/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collect_word.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 19:51:20 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/14 20:56:02 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Mask IFS whitespace inside quotes so it won't split later
// These bytes are control chars that shouldn't appear in normal input

# define MS_MASK_SPACE 0x1F
# define MS_MASK_TAB 0x1E
# define MS_MASK_NL 0x1D

static void ms_mask_ifs(char *s)
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

static char	*ms_expand_variable(t_shell *shell, char *str, int *idx)
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
	/* If no valid variable name characters found, treat $ as literal */
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

static char	*ms_collect_single_quotes(char *str, int *idx)
{
	int		start;
	char	*tmp;

	(*idx)++;
	start = *idx;
	while (str[*idx] && str[*idx] != '\'')
		(*idx)++;
	tmp = ft_substr(str, start, *idx - start);
	ms_mask_ifs(tmp);
	if (str[*idx] == '\'')
		(*idx)++;
	return (tmp);
}

static char	*ms_collect_double_quotes(t_shell *shell, char *str, int *idx,
										int allow_expansion)
{
	int		start;
	char	*buf;
	char	*tmp;
	char	*old_buf;

	(*idx)++;
	buf = ft_strdup("");
	while (str[*idx] && str[*idx] != '"')
	{
		if (str[*idx] == '$' && allow_expansion)
		{
			tmp = ms_expand_variable(shell, str, idx);
			ms_mask_ifs(tmp);
			old_buf = buf;
			buf = ft_strjoin(buf, tmp);
			free(old_buf);
			free(tmp);
		}
		else
		{
			start = *idx;
			while (str[*idx] && str[*idx] != '"' && !(str[*idx] == '$' && allow_expansion))
				(*idx)++;
			tmp = ft_substr(str, start, *idx - start);
			ms_mask_ifs(tmp);
			old_buf = buf;
			buf = ft_strjoin(buf, tmp);
			free(old_buf);
			free(tmp);
		}
	}
	if (str[*idx] == '"')
		(*idx)++;
	return (buf);
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

char *ms_collect_word(t_shell *shell, char *str, int *idx, int *was_quoted, int allow_expansion)
{
    char    *buf;
    char    *tmp;
    char    *old_buf;
	int		start;

    buf = ft_strdup("");
    *was_quoted = 0;  // Initialize quote flag

    while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
        && str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>')
    {
        if (str[*idx] == '\'')
        {
            *was_quoted = 1;
            tmp = ms_collect_single_quotes(str, idx);
        }
        else if (str[*idx] == '"')
        {
            *was_quoted = 1;
            tmp = ms_collect_double_quotes(shell, str, idx, allow_expansion);
        }
        else if (str[*idx] == '$' && allow_expansion)
        {
            tmp = ms_expand_variable(shell, str, idx);
        }
		else if (str[*idx] == '$' && !allow_expansion)
		{
			// Treat '$' as literal, collect it as plain text
			start = *idx;
			(*idx)++;
			while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
				&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>'
				&& str[*idx] != '\'' && str[*idx] != '"')
				(*idx)++;
			tmp = ft_substr(str, start, *idx - start);
		}
        else
        {
            // Treat '$' as normal char if expansion not allowed
            tmp = ms_collect_plain_word(str, idx);
        }
        old_buf = buf;
        buf = ft_strjoin(buf, tmp);
        free(old_buf);
        free(tmp);
    }
    return (buf);
}
