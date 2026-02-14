/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collect_word.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 19:51:20 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/14 15:37:59 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	if (str[*idx] == '\'')
		(*idx)++;
	return (tmp);
}

static char	*ms_collect_double_quotes(t_shell *shell, char *str, int *idx)
{
	int		start;
	char	*buf;
	char	*tmp;
	char	*old_buf;

	(*idx)++;
	buf = ft_strdup("");
	while (str[*idx] && str[*idx] != '"')
	{
		if (str[*idx] == '$')
		{
			tmp = ms_expand_variable(shell, str, idx);
			old_buf = buf;
			buf = ft_strjoin(buf, tmp);
			free(old_buf);
			free(tmp);
		}
		else
		{
			start = *idx;
			while (str[*idx] && str[*idx] != '"' && str[*idx] != '$')
				(*idx)++;
			tmp = ft_substr(str, start, *idx - start);
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
            //*was_quoted = 0;
            tmp = ms_collect_double_quotes(shell, str, idx);
        }
        else if (str[*idx] == '$' && allow_expansion)
        {
            tmp = ms_expand_variable(shell, str, idx);
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
