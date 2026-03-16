#include "minishell.h"

static void	ms_redir_error(char *target)
{
	char	*msg;
	char	*line;

	if (!target)
		return ;
	msg = ms_str_join_three(target, ": ", strerror(errno));
	if (!msg)
		return ;
	line = ms_str_join_three(msg, "\n", "");
	free (msg);
	if (!line)
		return ;
	write(STDERR_FILENO, line, ft_strlen(line));
	free(line);
}

static int	ms_open_output_file(t_redir *redir)
{
	int	fd;

	if (redir->type == REDIR_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		ms_redir_error(redir->target);
	return (fd);
}

static int ms_redir_ambiguous_error(char *target)
{
	if (target)
		ft_putstr_fd(target, STDERR_FILENO);
	ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
	return (-1);
}

int	ms_apply_redirections(t_redir *redirections)
{
	int	fd;

	while (redirections)
	{
		if (redirections->ambiguous)
			return (ms_redir_ambiguous_error(redirections->target));
		if (redirections->type == REDIR_IN)
		{
			fd = open(redirections->target, O_RDONLY);
			if (fd < 0)
			{
				ms_redir_error(redirections->target);
				return (-1);
			}
			if (dup2(fd, STDIN_FILENO) < 0)
			{
				perror("dup2");
				close(fd);
				return (-1);
			}
			close(fd);
		}
		else if (redirections->type == REDIR_OUT
			|| redirections->type == REDIR_APPEND)
		{
			fd = ms_open_output_file(redirections);
			if (fd < 0)
				return (-1);
			//dup2(fd, STDOUT_FILENO);
			if (dup2(fd, STDOUT_FILENO) < 0)
			{
				perror("dup2");
				close(fd);
				return (-1);
			}
			close(fd);
		}
		else if (redirections->type == REDIR_HEREDOC)
		{
			fd = redirections->heredoc_fd;
			if (fd < 0)
				return (-1);
			//dup2(fd, STDIN_FILENO);
			//close(fd);
			if (dup2(fd, STDIN_FILENO) < 0)
			{
				perror("dup2");
				close(fd);
				redirections->heredoc_fd = -1;
				return (-1);
			}
			close(fd);
			redirections->heredoc_fd = -1;
		}
		redirections = redirections->next;
	}
	return (0);
}
