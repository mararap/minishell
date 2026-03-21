/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_count_commands(t_command *cmd)
{
	int	count;

	count = 0;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

static int	ms_use_parent_builtin(t_command *command_list)
{
	if (!command_list->argv)
		return (0);
	if (!ms_is_builtin(command_list->argv[0]))
		return (0);
	return (ms_builtin_needs_parent(command_list->argv[0]));
}

static void	ms_prepare_pipeline_signals(t_shell *shell)
{
	if (!shell->is_interactive)
		return ;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

static int	ms_finish_pipeline(t_pipeline *pl)
{
	int	status;

	if (pl->prev_read >= 0 && pl->prev_read != STDIN_FILENO)
		close(pl->prev_read);
	status = ms_wait_for_children(pl->pids[pl->created - 1], pl->created,
			pl->shell->is_interactive);
	free(pl->pids);
	if (pl->shell->is_interactive)
		ms_setup_interactive_signals();
	return (status);
}

int	ms_execute_pipeline(t_shell *shell, t_command *command_list)
{
	t_pipeline	pl;
	int			status;

	if (ms_count_commands(command_list) == 1
		&& ms_use_parent_builtin(command_list))
		return (ms_run_builtin_parent(shell, command_list));
	pl.shell = shell;
	pl.command_list = command_list;
	pl.cmd = command_list;
	pl.pids = (pid_t *)ms_xmalloc(sizeof(pid_t)
			* ms_count_commands(command_list));
	pl.prev_read = STDIN_FILENO;
	pl.created = 0;
	ms_prepare_pipeline_signals(shell);
	status = ms_spawn_pipeline(&pl);
	if (status != 0)
		return (status);
	return (ms_finish_pipeline(&pl));
}
