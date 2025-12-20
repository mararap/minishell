#include "minishell.h"

static int	ms_open_output_file(t_redir *redir)
{
	int	fd;

	if (redir->type == REDIR_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		ms_perror(redir->target);
	return (fd);
}

static int	ms_apply_heredoc(t_redir *redir, t_shell *shell)
{
	int		pipe_fd[2];
	char	*line;

	(void)shell;
	if (pipe(pipe_fd) < 0)
	{
		ms_perror("pipe");
		return (-1);
	}
	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
			break ;
		if (ft_strncmp(line, redir->target,
				ft_strlen(redir->target) + 1) == 0)
		{
			free(line);
			break ;
		}
		write(pipe_fd[1], line, ft_strlen(line));
		write(pipe_fd[1], "\n", 1);
		free(line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}

int	ms_apply_redirections(t_redir *redirections, t_shell *shell)
{
	int	fd;

	while (redirections)
	{
		if (redirections->type == REDIR_IN)
		{
			fd = open(redirections->target, O_RDONLY);
			if (fd < 0)
				return (-1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redirections->type == REDIR_OUT
			|| redirections->type == REDIR_APPEND)
		{
			fd = ms_open_output_file(redirections);
			if (fd < 0)
				return (-1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redirections->type == REDIR_HEREDOC)
		{
			fd = ms_apply_heredoc(redirections, shell);
			if (fd < 0)
				return (-1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		redirections = redirections->next;
	}
	return (0);
}
