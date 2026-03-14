/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/24 21:22:37 by marapovi          #+#    #+#             */
/*   Updated: 2026/01/24 21:26:01 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*ms_token_new(t_token_type type, char *value, char *raw, int quoted)
{
	t_token	*tok;

	tok = (t_token *)ms_xmalloc(sizeof(t_token));
	tok->type = type;
	tok->value = value;
	tok->raw = raw;
	tok->quoted = quoted;
	tok->next = NULL;
	return (tok);
}

void	ms_token_add_back(t_token **list, t_token *new_tok)
{
	t_token	*iter;

	if (!*list)
	{
		*list = new_tok;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_tok;
}
