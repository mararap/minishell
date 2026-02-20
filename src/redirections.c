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

/* int	ms_check_redirections(t_redir *redirections)
{
	int		fd;
	t_redir	*r;
	int		has_error;

	has_error = 0;
	r = redirections;
	while (r)
	{
		if (r->type == REDIR_IN)
		{
			fd = open(r->target, O_RDONLY);
			if (fd < 0)
			{
				ms_perror(r->target);
				has_error = 1;
			}
			else
				close(fd);
		}
		else if (r->type == REDIR_OUT || r->type == REDIR_APPEND)
		{
			fd = ms_open_output_file(r);
			if (fd < 0)
				has_error = 1;
			else
				close(fd);
		}
		else if (r->type == REDIR_HEREDOC)
		{
			if (r->heredoc_fd < 0)
				has_error = 1;
		}
		r = r->next;
	}
	if (has_error)
		return (-1);
	else
		return (0);
} */

int	ms_apply_redirections(t_redir *redirections)
{
	int	fd;

	while (redirections)
	{
		if (redirections->type == REDIR_IN)
		{
			fd = open(redirections->target, O_RDONLY);
			if (fd < 0)
			{
				ms_perror(redirections->target);
				return (-1);
			}
			if (dup2(fd, STDIN_FILENO) < 0)
			{
				ms_perror("dup2");
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
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redirections->type == REDIR_HEREDOC)
		{
			fd = redirections->heredoc_fd;
			if (fd < 0)
				return (-1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		redirections = redirections->next;
	}
	return (0);
}
