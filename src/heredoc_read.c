/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ms_hd_read_line(t_shell *shell)
{
	char	*line;

	if (shell->is_interactive)
		return (readline(HEREDOC_PROMPT));
	line = ms_get_next_line(STDIN_FILENO);
	if (line)
		ms_chomp_eol(line);
	return (line);
}

void	ms_hd_warn_eof(t_redir *redir)
{
	char	*line_num;

	line_num = ft_itoa(redir->heredoc_line + 1);
	if (!line_num)
		return ;
	write(STDERR_FILENO, SHELL_NAME ": warning: here-document at line ",
		ft_strlen(SHELL_NAME) + 33);
	write(STDERR_FILENO, line_num, ft_strlen(line_num));
	write(STDERR_FILENO, " delimited by end-of-file (wanted `", 35);
	write(STDERR_FILENO, redir->target, ft_strlen(redir->target));
	write(STDERR_FILENO, "')\n", 3);
	free(line_num);
}

int	ms_hd_write_line(t_shell *shell, t_redir *redir, int wfd, char *line)
{
	char	*expanded;

	if (!redir->heredoc_expand)
	{
		write(wfd, line, ft_strlen(line));
		write(wfd, "\n", 1);
		free(line);
		return (0);
	}
	expanded = ms_expand_heredoc_line(shell, line);
	free(line);
	if (!expanded)
		return (1);
	write(wfd, expanded, ft_strlen(expanded));
	write(wfd, "\n", 1);
	free(expanded);
	return (0);
}

int	ms_hd_child_loop(t_shell *shell, t_redir *redir, int wfd)
{
	char	*line;
	int		cur_line;

	cur_line = redir->heredoc_line;
	while (1)
	{
		line = ms_hd_read_line(shell);
		cur_line++;
		if (!line)
		{
			ms_hd_warn_eof(redir);
			return (cur_line - redir->heredoc_line);
		}
		if (ft_strcmp(line, redir->target) == 0)
		{
			free(line);
			return (cur_line - redir->heredoc_line);
		}
		if (ms_hd_write_line(shell, redir, wfd, line) != 0)
			return (1);
	}
}