/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/22 15:25:10 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_dup_and_close(int from, int to)
{
	if (from != to)
	{
		if (dup2(from, to) < 0)
			return (-1);
		close(from);
	}
	return (0);
}

static void	ms_child_exit(t_exec_ctx *ctx, int status)
{
	if (ctx->shell && ctx->shell->current_line)
	{
		free(ctx->shell->current_line);
		ctx->shell->current_line = NULL;
	}
	if (ctx->cmd_list)
		ms_free_command_list(ctx->cmd_list);
	if (ctx->shell)
		ms_free_shell(ctx->shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	exit(status);
}

static void	ms_close_heredocs(t_command *cmd_list, t_command *current)
{
	t_redir	*r;

	while (cmd_list)
	{
		r = cmd_list->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC && r->heredoc_fd >= 0
				&& cmd_list != current)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		cmd_list = cmd_list->next;
	}
}

static int	ms_dup_child_fds(t_exec_ctx *ctx, t_command *cmd)
{
	if (ms_dup_and_close(ctx->prev_read, STDIN_FILENO) < 0)
	{
		ctx->prev_read = -1;
		return (-1);
	}
	ctx->prev_read = -1;
	if (!cmd->next)
		return (0);
	if (ms_dup_and_close(ctx->pipe_fd[1], STDOUT_FILENO) < 0)
	{
		ctx->pipe_fd[1] = -1;
		return (-1);
	}
	ctx->pipe_fd[1] = -1;
	return (0);
}

void	ms_execute_child(t_exec_ctx *ctx, t_command *cmd)
{
	int	status;

	ms_setup_child_signals();
	if (ms_dup_child_fds(ctx, cmd) < 0)
		ms_child_exit(ctx, 1);
	ms_close_heredocs(ctx->cmd_list, cmd);
	if (ms_apply_redirections(cmd->redirections) < 0)
		ms_child_exit(ctx, 1);
	if (!cmd->argv || !cmd->argv[0])
		ms_child_exit(ctx, 0);
	ms_update_underscore(ctx->shell, cmd->argv[0]);
	if (ms_is_builtin(cmd->argv[0]))
		ms_child_exit(ctx, ms_run_builtin_child(ctx->shell, cmd->argv));
	status = ms_exec_external_command(ctx->shell, cmd->argv);
	ms_child_exit(ctx, status);
}

int	ms_fork_and_execute(t_exec_ctx *ctx, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		ms_setup_child_signals();
		if (cmd->next)
		{
			close(ctx->pipe_fd[0]);
			ctx->pipe_fd[0] = -1;
		}
		if (ctx->pids_to_free)
			free(ctx->pids_to_free);
		ms_execute_child(ctx, cmd);
	}
	return (pid);
}
