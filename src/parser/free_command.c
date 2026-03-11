/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 15:47:37 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 19:43:09 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
