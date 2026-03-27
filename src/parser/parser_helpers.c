/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 20:53:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/27 20:35:40 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char	*ms_token_to_str(t_token_type type)
{
	if (type == TOKEN_PIPE)
		return ("|");
	if (type == TOKEN_REDIR_IN)
		return ("<");
	if (type == TOKEN_REDIR_OUT)
		return (">");
	if (type == TOKEN_REDIR_APPEND)
		return (">>");
	if (type == TOKEN_HEREDOC)
		return ("<<");
	return ("newline");
}

void	ms_print_syntax_error(t_token *tok)
{
	const char	*unexpected;

	if (!tok)
		unexpected = "newline";
	else
		unexpected = ms_token_to_str(tok->type);
	ft_putstr_fd("syntax error near unexpected token `",
		STDERR_FILENO);
	ft_putstr_fd((char *)unexpected, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
}

t_redir	*ms_create_redir(int type, char *target, int heredoc_expand,
	int ambiguous)
{
	t_redir	*redir;

	redir = (t_redir *)ms_xmalloc(sizeof(t_redir));
	redir->type = type;
	redir->target = target;
	redir->ambiguous = ambiguous;
	redir->heredoc_fd = -1;
	redir->heredoc_expand = heredoc_expand;
	redir->next = NULL;
	return (redir);
}

void	ms_redir_add_back(t_redir **list, t_redir *new_redir)
{
	t_redir	*iter;

	if (!*list)
	{
		*list = new_redir;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_redir;
}

int	ms_token_to_redir_type(t_token_type t)
{
	if (t == TOKEN_REDIR_IN)
		return (REDIR_IN);
	if (t == TOKEN_REDIR_OUT)
		return (REDIR_OUT);
	if (t == TOKEN_REDIR_APPEND)
		return (REDIR_APPEND);
	if (t == TOKEN_HEREDOC)
		return (REDIR_HEREDOC);
	return (-1);
}
