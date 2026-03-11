/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 20:13:43 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 15:14:54 by marapovi         ###   ########.fr       */
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

static int	ms_parse_pipe_error(t_command *cmds)
{
	ft_putstr_fd(SHELL_NAME ": syntax error near unexpected token `|'\n",
		STDERR_FILENO);
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
		*cursor = (*cursor)->next;
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
			if (ms_parse_pipe_error(cmds))
				return (NULL);
		}
		if (ms_parse_one_command(&cmds, &cursor))
			return (NULL);
	}
	return (cmds);
}

static void	ms_free_argv(t_command *command_list)
{
	size_t	i;

	i = 0;
	while (command_list->argv[i])
	{
		free(command_list->argv[i]);
		i++;
	}
	free(command_list->argv);
}

void	ms_free_command_list(t_command *command_list)
{
	t_command	*next_cmd;
	t_redir		*redir;
	t_redir		*next_redir;

	while (command_list)
	{
		next_cmd = command_list->next;
		if (command_list->argv)
			ms_free_argv(command_list);
		redir = command_list->redirections;
		while (redir)
		{
			next_redir = redir->next;
			free(redir->target);
			if (redir->type == REDIR_HEREDOC && redir->heredoc_fd >= 0)
				close(redir->heredoc_fd);
			free(redir);
			redir = next_redir;
		}
		free(command_list);
		command_list = next_cmd;
	}
}
