/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_prepare.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_close_all_heredocs(t_command *cmds)
{
	t_redir	*r;

	while (cmds)
	{
		r = cmds->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC && r->heredoc_fd >= 0)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		cmds = cmds->next;
	}
}

int	ms_prepare_one_heredoc(t_shell *shell, t_command *cmds, t_redir *redir,
		int *hd_line_num)
{
	int	fd;
	int	lines_read;

	redir->heredoc_line = *hd_line_num;
	fd = ms_build_one_heredoc(shell, cmds, redir, &lines_read);
	if (fd == -2)
		return (130);
	if (fd < 0)
		return (1);
	redir->heredoc_fd = fd;
	*hd_line_num += lines_read;
	shell->input_line_num++;
	return (0);
}

static int	ms_prepare_command_heredocs(t_shell *shell, t_command *cmds,
		t_command *cmd, int *hd_line_num)
{
	t_redir	*r;
	int		status;

	r = cmd->redirections;
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			status = ms_prepare_one_heredoc(shell, cmds, r, hd_line_num);
			if (status != 0)
				return (status);
		}
		r = r->next;
	}
	return (0);
}

int	ms_prepare_heredocs(t_shell *shell, t_command *cmds)
{
	int	status;
	int	hd_line_num;

	hd_line_num = 1;
	while (cmds)
	{
		status = ms_prepare_command_heredocs(shell, cmds, cmds, &hd_line_num);
		if (status == 130)
			g_signal_number = SIGINT;
		if (status != 0)
		{
			if (status == 1)
				perror("heredoc");
			ms_close_all_heredocs(cmds);
			return (status);
		}
		cmds = cmds->next;
	}
	return (0);
}
