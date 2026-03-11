#include "minishell.h"

static int	ms_status_to_exit(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

static int	ms_wait_for_children(pid_t last_pid, int count, int interactive)
{
	int		status;
	int		last_status;
	int		waited;
	pid_t	pid;

	last_status = 0;
	waited = 0;
	while (waited < count)
	{
		pid = waitpid(-1, &status, 0);
		if (pid == -1)
		{
			if (errno == EINTR)
				continue;
			break;
		}
		if (pid == last_pid)
		{
		if (interactive && WIFSIGNALED(status))
		{
			int sig = WTERMSIG(status);
			if (sig == SIGINT)
				write(STDERR_FILENO, "\n", 1);
			else if (sig == SIGQUIT)
				write(STDERR_FILENO, "Quit (core dumped)\n", 19);
		}
		last_status = ms_status_to_exit(status);
		}
		waited++;
	}
	return (last_status);
}

int	ms_create_pipe_if_needed(t_command *cmd, int pipe_fd[2])
{
	if (!cmd->next)
		return (0);
	if (pipe(pipe_fd) < 0)
	{
		perror("pipe\n");
		return (-1);
	}
	return (1);
}

//keep in executor
void	ms_update_parent_fds(int *prev_read, t_command *cmd, int pipe_fd[2])
{
	if (*prev_read != STDIN_FILENO)
		close(*prev_read);
	if (cmd->next)
	{
		close(pipe_fd[1]);
		*prev_read = pipe_fd[0];
	}
}

//keep in executor
int	ms_pipeline_error(pid_t *pids, int prev_read)
{
	if (pids)
		free(pids);
	if (prev_read != STDIN_FILENO)
		close(prev_read);
	perror("pipeline\n");
	return (1);
}

static int	ms_abort_pipeline(t_shell *shell, pid_t *pids, int created,
				int prev_read, int pipe_fd[2])
{
	int	i;

	if (prev_read >= 0 && prev_read != STDIN_FILENO)
		close(prev_read);
	if (pipe_fd[0] >= 0)
		close(pipe_fd[0]);
	if (pipe_fd[1] >= 0)
		close(pipe_fd[1]);
	i = 0;
	while (i < created)
	{
		if (pids[i] > 0)
			kill(pids[i], SIGTERM);
		i++;
	}
	if (created > 0)
		ms_wait_for_children(pids[created - 1], created, 0);
	free(pids);
	if (shell->is_interactive)
		ms_setup_interactive_signals();
	return (1);
}

int	ms_execute_pipeline(t_shell *shell, t_command *command_list)
{
	int			cmd_count;
	t_command	*cmd;
	int			prev_read;
	int			pipe_fd[2];
	pid_t		*pids;
	pid_t		last_pid;
	int			i;
	int			status;

	cmd_count = 0;
	cmd = command_list;
	while (cmd)
	{
		cmd_count++;
		cmd = cmd->next;
	}
	if (cmd_count == 1 && command_list->argv
		&& ms_is_builtin(command_list->argv[0]))
		return (ms_run_builtin_parent(shell, command_list));

	pids = (pid_t *)ms_xmalloc(sizeof(pid_t) * cmd_count);

	if (shell->is_interactive)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
	}

	prev_read = STDIN_FILENO;
	cmd = command_list;
	i = 0;
	while (cmd)
	{
		pipe_fd[0] = -1;
		pipe_fd[1] = -1;
		if (cmd->next)
		{
			if (pipe(pipe_fd) < 0)
			{
				perror("pipe\n");
				return (ms_abort_pipeline(shell, pids, i, prev_read, pipe_fd));
			}
		}
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("fork\n");
			return (ms_abort_pipeline(shell, pids, i, prev_read, pipe_fd));
		}
		if (pids[i] == 0)
		{
			free(pids);
			if (pipe_fd[0] >= 0)
				close(pipe_fd[0]);
			ms_execute_child(shell, command_list, cmd, prev_read, \
				(cmd->next ? pipe_fd[1] : STDOUT_FILENO));
			exit(1);
		}
		if (prev_read != STDIN_FILENO)
			close(prev_read);
		if (pipe_fd[1] >= 0)
			close(pipe_fd[1]);
		prev_read = pipe_fd[0];
		cmd = cmd->next;
		i++;
	}

	last_pid = pids[cmd_count - 1];
	if (prev_read >= 0 && prev_read != STDIN_FILENO)
		close(prev_read);

	status = ms_wait_for_children(last_pid, cmd_count, shell->is_interactive);
	free(pids);
	if (shell->is_interactive)
		ms_setup_interactive_signals();
	return (status);
}


