/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unclosed_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 10:14:07 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/23 10:15:22 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*ms_syntax_error(t_shell *shell)
{
	ms_print_syntax_error(NULL);
	shell->last_exit_status = 2;
	if (!shell->is_interactive)
		shell->should_exit = 1;
	return (NULL);
}

static int	ms_skip_quote(char *line, int *idx, char quote)
{
	(*idx)++;
	while (line[*idx] && line[*idx] != quote)
		(*idx)++;
	if (!line[*idx])
		return (1);
	(*idx)++;
	return (0);
}

int	ms_has_unclosed_quotes(char *line)
{
	int	i;

	i = 0;
	while (line && line[i])
	{
		if (line[i] == '\'')
		{
			if (ms_skip_quote(line, &i, '\''))
				return (1);
		}
		else if (line[i] == '"' || (line[i] == '$' && line[i + 1] == '"'))
		{
			if (line[i] == '$')
				i++;
			if (ms_skip_quote(line, &i, '"'))
				return (1);
		}
		else
			i++;
	}
	return (0);
}
