/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_spawn.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_create_pipe_if_needed(t_pipeline *pl)
{
	if (!pl->cmd->next)
		return (0);
	if (pipe(pl->pipe_fd) < 0)
	{
		perror("pipe");
		return (-1);
	}
	return (1);
}

static void	ms_update_parent_fds(t_pipeline *pl)
{
	if (pl->prev_read >= 0 && pl->prev_read != STDIN_FILENO)
	{
		close(pl->prev_read);
		pl->prev_read = -1;
	}
	if (pl->cmd->next)
	{
		close(pl->pipe_fd[1]);
		pl->pipe_fd[1] = -1;
		pl->prev_read = pl->pipe_fd[0];
	}
}

static int	ms_abort_pipeline(t_pipeline *pl)
{
	int	i;

	if (pl->prev_read >= 0 && pl->prev_read != STDIN_FILENO)
		close(pl->prev_read);
	if (pl->pipe_fd[0] >= 0)
		close(pl->pipe_fd[0]);
	if (pl->pipe_fd[1] >= 0)
		close(pl->pipe_fd[1]);
	i = 0;
	while (i < pl->created)
	{
		if (pl->pids[i] > 0)
			kill(pl->pids[i], SIGTERM);
		i++;
	}
	if (pl->created > 0)
		ms_wait_for_children(pl->pids[pl->created - 1], pl->created, 0);
	free(pl->pids);
	if (pl->shell->is_interactive)
		ms_setup_interactive_signals();
	return (1);
}

static pid_t	ms_launch_pipeline_cmd(t_pipeline *pl)
{
	t_exec_ctx	ctx;
	pid_t		pid;

	if (ms_create_pipe_if_needed(pl) < 0)
		return (-1);
	ctx.shell = pl->shell;
	ctx.cmd_list = pl->command_list;
	ctx.prev_read = pl->prev_read;
	ctx.pipe_fd[0] = pl->pipe_fd[0];
	ctx.pipe_fd[1] = pl->pipe_fd[1];
	ctx.pids_to_free = pl->pids;
	pid = ms_fork_and_execute(&ctx, pl->cmd);
	if (pid < 0)
		return (-1);
	ms_update_parent_fds(pl);
	return (pid);
}

int	ms_spawn_pipeline(t_pipeline *pl)
{
	while (pl->cmd)
	{
		pl->pipe_fd[0] = -1;
		pl->pipe_fd[1] = -1;
		pl->pids[pl->created] = ms_launch_pipeline_cmd(pl);
		if (pl->pids[pl->created] < 0)
			return (ms_abort_pipeline(pl));
		pl->cmd = pl->cmd->next;
		pl->created++;
	}
	return (0);
}
