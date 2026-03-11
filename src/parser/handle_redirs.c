/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 16:30:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 19:46:22 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_redir_type_or_error(t_token *tok, int *type)
{
	*type = ms_token_to_redir_type(tok->type);
	if (*type < 0)
	{
		ms_print_syntax_error(tok);
		return (-1);
	}
	return (0);
}

static int	ms_redir_target_or_error(t_token **cursor)
{
	*cursor = (*cursor)->next;
	if (!*cursor)
	{
		ms_print_syntax_error(NULL);
		return (-1);
	}
	if ((*cursor)->type != TOKEN_WORD)
	{
		ms_print_syntax_error(*cursor);
		return (-1);
	}
	return (0);
}

static int	ms_redir_expand_flag(int type, t_token *target)
{
	if (type == REDIR_HEREDOC)
		return (target->quoted == 0);
	return (0);
}

int	ms_process_redir_token(t_command *cmd, t_token **cursor, t_token *tok)
{
	int	type;
	int	hd_expand;

	if (ms_redir_type_or_error(tok, &type) < 0)
		return (-1);
	if (ms_redir_target_or_error(cursor) < 0)
		return (-1);
	hd_expand = ms_redir_expand_flag(type, *cursor);
	ms_unmask_ifs((*cursor)->value);
	ms_redir_add_back(&cmd->redirections, ms_create_redir(type,
			(*cursor)->value, hd_expand));
	(*cursor)->value = NULL;
	*cursor = (*cursor)->next;
	return (0);
}
