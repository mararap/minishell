/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 17:22:15 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 19:43:54 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_fill_command(t_command *cmd, t_token **cursor)
{
	t_token	*tok;

	while (*cursor && (*cursor)->type != TOKEN_PIPE)
	{
		tok = *cursor;
		if (tok->type == TOKEN_WORD)
			ms_handle_word(cmd, cursor, tok);
		else if (ms_process_redir_token(cmd, cursor, tok) < 0)
			return (-1);
	}
	return (0);
}
