/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_redir_sys_error(char *target)
{
	char	*msg;
	char	*line;

	if (!target)
		return ;
	msg = ms_str_join_three(target, ": ", strerror(errno));
	if (!msg)
		return ;
	line = ms_str_join_three(msg, "\n", "");
	free(msg);
	if (!line)
		return ;
	write(STDERR_FILENO, line, ft_strlen(line));
	free(line);
}

static int	ms_apply_input_redir(t_redir *redir)
{
	int	fd;

	fd = open(redir->target, O_RDONLY);
	if (fd < 0)
	{
		ms_redir_sys_error(redir->target);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		close(fd);
		perror("dup2");
		return (-1);
	}
	close(fd);
	return (0);
}

static int	ms_apply_output_redir(t_redir *redir)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;	
	if (redir->type == REDIR_OUT)
		flags |= O_TRUNC;	
	else
		flags |= O_APPEND;
	fd = open(redir->target, flags, 0666);	
	if (fd < 0)
	{
		ms_redir_sys_error(redir->target);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		close(fd);
		perror("dup2");
		return (-1);
	}
	close(fd);
	return (0);
}

static int	ms_apply_heredoc_redir(t_redir *redir)
{
	if (redir->heredoc_fd < 0)
 		return (-1);
	if (dup2(redir->heredoc_fd, STDIN_FILENO) < 0)
	{
		close(redir->heredoc_fd);
		redir->heredoc_fd = -1;
		perror("dup2");
		return (-1);
	}
	close(redir->heredoc_fd);
	redir->heredoc_fd = -1;
	return (0);
}

int	ms_apply_redirections(t_redir *redirections)
{
	while (redirections)
	{
		if (redirections->ambiguous)
		{
			write(STDERR_FILENO, redirections->target,
				ft_strlen(redirections->target));
			write(STDERR_FILENO, ": ambiguous redirect\n", 22);
			return (-1);
 		}
		if (redirections->type == REDIR_IN
			&& ms_apply_input_redir(redirections) < 0)
			return (-1);
		if ((redirections->type == REDIR_OUT
				|| redirections->type == REDIR_APPEND)
			&& ms_apply_output_redir(redirections) < 0)
			return (-1);
		if (redirections->type == REDIR_HEREDOC
			&& ms_apply_heredoc_redir(redirections) < 0)
			return (-1);
 		redirections = redirections->next;
 	}
 	return (0);
}
