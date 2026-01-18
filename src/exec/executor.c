#include "minishell.h"

//keep in executor
static int	ms_wait_for_children(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status = 0;

	for (i = 0; i < count; i++)
	{
		if (waitpid(pids[i], &status, 0) > 0)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
	}
	return (last_status);
}

//keep in executor
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

//keep in executor
static int	ms_should_run_builtin_parent(t_command *cmd)
{
	if (!cmd || cmd->next || !cmd->argv)
		return (0);
	return (ms_is_builtin(cmd->argv[0]));
}

//keep in executor
int	ms_create_pipe_if_needed(t_command *cmd, int pipe_fd[2])
{
	if (!cmd->next)
		return (0);
	if (pipe(pipe_fd) < 0)
	{
		ms_perror("pipe");
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
	ms_perror("pipeline");
	return (1);
}

//keep in executor
int	ms_execute_pipeline(t_shell *shell, t_command *cmds)
{
	int			i;
	int			cmd_count;
	int			prev_read;
	int			pipe_fd[2];
	pid_t		*pids;
	t_command	*cmd;

	cmd_count = ms_count_commands(cmds);
	if (ms_should_run_builtin_parent(cmds))
		return (ms_run_builtin_parent(shell, cmds));
	pids = ms_xmalloc(sizeof(pid_t) * cmd_count);
	prev_read = STDIN_FILENO;
	cmd = cmds;
	i = 0;
	while (cmd)
	{
		if (ms_create_pipe_if_needed(cmd, pipe_fd) < 0)
			return (ms_pipeline_error(pids, prev_read));
		pids[i] = ms_fork_and_execute(shell, cmd, prev_read, pipe_fd);
		if (pids[i++] < 0)
			return (ms_pipeline_error(pids, prev_read));
		ms_update_parent_fds(&prev_read, cmd, pipe_fd);
		cmd = cmd->next;
	}
	return (ms_wait_for_children(pids, cmd_count));
}


