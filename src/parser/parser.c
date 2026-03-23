/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 20:13:43 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 15:50:44 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_command	*ms_command_new(void)
{
	t_command	*cmd;

	cmd = (t_command *)ms_xmalloc(sizeof(t_command));
	cmd->argv = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	ms_command_add_back(t_command **list, t_command *new_cmd)
{
	t_command	*iter;

	if (!*list)
	{
		*list = new_cmd;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_cmd;
}

static int	ms_parse_pipe_error(t_command *cmds, t_token *tok)
{
	ms_print_syntax_error(tok);
	ms_free_command_list(cmds);
	return (1);
}

static int	ms_parse_one_command(t_command **cmds, t_token **cursor)
{
	t_command	*cmd;

	cmd = ms_command_new();
	if (ms_fill_command(cmd, cursor) < 0)
	{
		ms_free_command_list(cmd);
		ms_free_command_list(*cmds);
		return (1);
	}
	ms_command_add_back(cmds, cmd);
	if (*cursor && (*cursor)->type == TOKEN_PIPE)
	{
		*cursor = (*cursor)->next;
		if (!*cursor)
		{
			ms_print_syntax_error(NULL);
			ms_free_command_list(*cmds);
			return (1);
		}
	}
	return (0);
}

t_command	*ms_parse_tokens(t_token *token_list)
{
	t_command	*cmds;
	t_token		*cursor;

	cmds = NULL;
	cursor = token_list;
	while (cursor)
	{
		if (cursor->type == TOKEN_PIPE)
		{
			if (ms_parse_pipe_error(cmds, cursor))
				return (NULL);
		}
		if (ms_parse_one_command(&cmds, &cursor))
			return (NULL);
	}
	return (cmds);
}
