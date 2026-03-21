/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_wait.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_status_to_exit(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

static void	ms_report_child_signal(int status, int interactive)
{
	int	sig;

	if (!interactive || !WIFSIGNALED(status))
		return ;
	sig = WTERMSIG(status);
	if (sig == SIGINT)
		write(STDERR_FILENO, "\n", 1);
	else if (sig == SIGQUIT)
		write(STDERR_FILENO, "Quit (core dumped)\n", 19);
}

static void	ms_store_last_status(pid_t pid, t_wait_state *state, int status)
{
	if (pid != state->last_pid)
		return ;
	ms_report_child_signal(status, state->interactive);
	state->last_status = ms_status_to_exit(status);
}

int	ms_wait_for_children(pid_t last_pid, int count, int interactive)
{
	t_wait_state	state;
	int				status;
	int				waited;
	pid_t				pid;

	state.last_pid = last_pid;
	state.interactive = interactive;
	state.last_status = 0;
	waited = 0;
	while (waited < count)
	{
		pid = waitpid(-1, &status, 0);
		if (pid == -1 && errno == EINTR)
			continue ;
		if (pid == -1)
			break ;
		ms_store_last_status(pid, &state, status);
		waited++;
	}
	return (state.last_status);
}