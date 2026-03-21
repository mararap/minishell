/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_syntax.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_discard_one_heredoc_body(const char *delim)
{
	char	*line;

	if (!delim)
		return ;
	while (1)
	{
		line = ms_get_next_line(STDIN_FILENO);
		if (!line)
			return ;
		ms_chomp_eol(line);
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			return ;
		}
		free(line);
	}
}

void	ms_discard_pending_heredocs(t_token *tokens)
{
	t_token	*next;

	while (tokens)
	{
		if (tokens->type == TOKEN_HEREDOC)
		{
			next = tokens->next;
			if (next && next->type == TOKEN_WORD && next->value)
				ms_discard_one_heredoc_body(next->value);
			tokens = next;
		}
		if (tokens)
			tokens = tokens->next;
	}
}
