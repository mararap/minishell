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
static void	ms_store_last_status(pid_t pid, pid_t last_pid, int status,
		int interactive, int *last_status)
{
	if (pid != last_pid)
		return ;
	ms_report_child_signal(status, interactive);
	*last_status = ms_status_to_exit(status);
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
		if (pid == -1 && errno == EINTR)
			continue ;
		if (pid == -1)
			break ;
		ms_store_last_status(pid, last_pid, status, interactive, &last_status);
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
static pid_t	ms_launch_pipeline_cmd(t_shell *shell, t_command *command_list,
		t_command *cmd, int *prev_read, int pipe_fd[2])
{
	pid_t	pid;

	if (ms_create_pipe_if_needed(cmd, pipe_fd) < 0)
		return (-1);
	pid = ms_fork_and_execute(shell, command_list, cmd, *prev_read, pipe_fd);
	if (pid < 0)
		return (-1);
	ms_update_parent_fds(prev_read, cmd, pipe_fd);
	return (pid);
}
static int	ms_finish_pipeline(t_shell *shell, pid_t *pids, int cmd_count,
		int prev_read)
{
	int	status;

	if (prev_read >= 0 && prev_read != STDIN_FILENO)
		close(prev_read);
	status = ms_wait_for_children(pids[cmd_count - 1], cmd_count,
			shell->is_interactive);
	free(pids);
	if (shell->is_interactive)
		ms_setup_interactive_signals();
	return (status);
}
static int	ms_spawn_pipeline(t_shell *shell, t_command *command_list,
		t_command *cmd, pid_t *pids)
{
	int	prev_read;
	int	pipe_fd[2];
	int	i;

	prev_read = STDIN_FILENO;
	i = 0;
	while (cmd)
	{
		pipe_fd[0] = -1;
		pipe_fd[1] = -1;
		pids[i] = ms_launch_pipeline_cmd(shell, command_list, cmd, &prev_read,
				pipe_fd);
		if (pids[i] < 0)
			return (ms_abort_pipeline(shell, pids, i, prev_read, pipe_fd));
		cmd = cmd->next;
		i++;
	}
	return (ms_finish_pipeline(shell, pids, i, prev_read));
}
int	ms_execute_pipeline(t_shell *shell, t_command *command_list)
{
	int		cmd_count;
	pid_t	*pids;

	cmd_count = ms_count_commands(command_list);
	if (cmd_count == 1 && ms_use_parent_builtin(command_list))
		return (ms_run_builtin_parent(shell, command_list));
	pids = (pid_t *)ms_xmalloc(sizeof(pid_t) * cmd_count);
	ms_prepare_pipeline_signals(shell);
	return (ms_spawn_pipeline(shell, command_list, command_list, pids));
}
