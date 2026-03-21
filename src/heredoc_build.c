/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_build.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_hd_child_exit(t_hd_job *job, int status)
{
	close(job->wfd);
	if (job->shell->current_line)
	{
		free(job->shell->current_line);
		job->shell->current_line = NULL;
	}
	ms_free_command_list(job->cmds);
	ms_free_shell(job->shell);
	free(job->path);
	exit(status);
}

void	ms_hd_run_child(t_hd_job *job)
{
	int	status;

	ms_setup_child_signals();
	status = ms_hd_child_loop(job->shell, job->redir, job->wfd);
	ms_hd_child_exit(job, status);
}

int	ms_hd_wait_child(pid_t pid, int *st)
{
	if (pid < 0)
		return (-1);
	while (waitpid(pid, st, 0) < 0 && errno == EINTR)
		;
	return (0);
}

int	ms_hd_finalize_build(t_hd_job *job)
{
	int	rfd;

	ms_restore_signals(job->shell);
	close(job->wfd);
	if (WIFSIGNALED(job->st) && WTERMSIG(job->st) == SIGINT)
	{
		unlink(job->path);
		free(job->path);
		return (-2);
	}
	if (WIFEXITED(job->st))
		*job->lines_read = WEXITSTATUS(job->st);
	rfd = open(job->path, O_RDONLY | O_CLOEXEC);
	unlink(job->path);
	free(job->path);
	if (rfd < 0)
		return (-1);
	return (rfd);
}

int	ms_build_one_heredoc(t_shell *shell, t_command *cmds, t_redir *redir,
		int *lines_read)
{
	t_hd_job	job;
	pid_t		pid;

	*lines_read = 0;
	job.shell = shell;
	job.cmds = cmds;
	job.redir = redir;
	job.lines_read = lines_read;
	job.path = NULL;
	job.wfd = ms_hd_open_tmp(&job.path);
	if (job.wfd < 0)
		return (-1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == 0)
		ms_hd_run_child(&job);
	if (ms_hd_wait_child(pid, &job.st) < 0)
	{
		ms_restore_signals(shell);
		close(job.wfd);
		unlink(job.path);
		free(job.path);
		return (-1);
	}
	return (ms_hd_finalize_build(&job));
}