/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 11:37:31 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/11 11:53:20 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_parent_signals_heredoc(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

static void	ms_restore_signals(t_shell *shell)
{
	if (shell->is_interactive)
		ms_setup_interactive_signals();
	else
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
}

static char	*ms_hd_make_path(int n)
{
	char	*num;
	char	*path;

	num = ft_itoa(n);
	if (!num)
		return (NULL);
	path = ms_str_join_three("/tmp/minishell_hd_", num, "");
	free(num);
	return (path);
}

static int	ms_hd_open_tmp(char **out_path)
{
	static int	counter = 0;
	char		*path;
	int			fd;

	while (counter < 1000000)
	{
		path = ms_hd_make_path(counter);
		counter++;
		if (!path)
			return (-1);
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0600);
		if (fd >= 0)
		{
			*out_path = path;
			return (fd);
		}
		free(path);
		if (errno != EEXIST)
			return (-1);
	}
	return (-1);
}

static int	ms_hd_append_char(char **buf, size_t *len, size_t *cap, char c)
{
	char	*newbuf;
	size_t	i;

	if (*len + 2 > *cap)
	{
		*cap = (*cap * 2) + 64;
		newbuf = (char *)malloc(*cap);
		if (!newbuf)
			return (-1);
		i = 0;
		while (i < *len)
		{
			newbuf[i] = (*buf)[i];
			i++;
		}
		free(*buf);
		*buf = newbuf;
	}
	(*buf)[*len] = c;
	*len = *len + 1;
	(*buf)[*len] = '\0';
	return (0);
}

static int	ms_hd_append_str(char **buf, size_t *len, size_t *cap, char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (ms_hd_append_char(buf, len, cap, s[i]) < 0)
			return (-1);
		i++;
	}
	return (0);
}

static char	*ms_expand_heredoc_line(t_shell *shell, char *line)
{
	char	*buf;
	size_t	len;
	size_t	cap;
	size_t	i;
	size_t	j;
	char	*name;
	char	*tmp;
	char	*val;

	cap = 64;
	buf = (char *)malloc(cap);
	if (!buf)
		return (NULL);
	buf[0] = '\0';
	len = 0;
	i = 0;
	while (line[i])
	{
		if (line[i] == '\\' && line[i + 1] == '$')
		{
			if (ms_hd_append_char(&buf, &len, &cap, '$') < 0)
				return (free(buf), NULL);
			i += 2;
		}
		else if (line[i] == '$' && line[i + 1] == '?')
		{
			tmp = ft_itoa(shell->last_exit_status);
			if (!tmp || ms_hd_append_str(&buf, &len, &cap, tmp) < 0)
				return (free(tmp), free(buf), NULL);
			free(tmp);
			i += 2;
		}
		else if (line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			j = i + 1;
			while (line[j] && (ft_isalnum(line[j]) || line[j] == '_'))
				j++;
			name = ft_substr(line, i + 1, j - (i + 1));
			if (!name)
				return (free(buf), NULL);
			val = ms_env_get_value(shell->env_list, name);
			free(name);
			if (val && ms_hd_append_str(&buf, &len, &cap, val) < 0)
				return (free(buf), NULL);
			i = j;
		}
		else
		{
			if (ms_hd_append_char(&buf, &len, &cap, line[i]) < 0)
				return (free(buf), NULL);
			i++;
		}
	}
	return (buf);
}

static void ms_hd_chomp_eol(char *s)
{
	size_t len;

	if(!s)
		return ;
	len = ft_strlen(s);
	while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
	{
		s[len - 1] = '\0';
		len--;
	}
}

static char *ms_hd_read_line(t_shell *shell)
{
	char *line;

	if(shell->is_interactive)
		return (readline(HEREDOC_PROMPT));
	line = ms_get_next_line(STDIN_FILENO);
	if (line)
		ms_hd_chomp_eol(line);
	return(line);
}

static int	ms_hd_child_loop(t_shell *shell, t_redir *r, int wfd)
{
	char	*line;
	char	*expanded;
	char	*line_num_str;
	int		cur_line;

	cur_line = r->heredoc_line;
	while (1)
	{
		line = ms_hd_read_line(shell);
		cur_line++;
		if (!line)
		{
			write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
			write(STDERR_FILENO, ": warning:", 10);
			write(STDERR_FILENO, " here-document at line ", 23);
			line_num_str = ft_itoa(r->heredoc_line + 1);
			write(STDERR_FILENO, line_num_str, ft_strlen(line_num_str));
			free(line_num_str);
			write(STDERR_FILENO, " delimited ", 11);
			write(STDERR_FILENO, "by end-of-file (wanted `", 24);
			write(STDERR_FILENO, r->target, ft_strlen(r->target));
			write(STDERR_FILENO, "')\n", 3);
			break ;
		}
		//always compare RAW input with delimiter
		if (ft_strcmp(line, r->target) == 0)
		{
			free(line);
			break ;
		}
		//expand for writing (if needed)
		if (r->heredoc_expand)
		{
			expanded = ms_expand_heredoc_line(shell, line);
			free(line);
			if (!expanded)
				return (1);
			write(wfd, expanded, ft_strlen(expanded));
			write(wfd, "\n", 1);
			free(expanded);
		}
		else
		{
			write(wfd, line, ft_strlen(line));
			write(wfd, "\n", 1);
			free(line);
		}
	}
	return (cur_line - r->heredoc_line);
}

/* returns: fd >= 0 success, -2 SIGINT, -1 error */
static int	ms_build_one_heredoc(t_shell *shell, t_command *cmds,
	t_redir *r, int *lines_read)
{
	char	*path;
	int		wfd;
	int		rfd;
	pid_t	pid;
	int		st;

	path = NULL;
	*lines_read = 0;
	wfd = ms_hd_open_tmp(&path);
	if (wfd < 0)
		return (-1);
	ms_parent_signals_heredoc();
	pid = fork();
	if (pid == 0)
	{
		ms_setup_child_signals();
		st = ms_hd_child_loop(shell, r, wfd);
		close(wfd);
		if (shell->current_line)
		{
			free(shell->current_line);
			shell->current_line = NULL;
		}
		ms_free_command_list(cmds);
		ms_free_shell(shell);
		free(path);
		exit(st);
	}
	if (pid < 0)
		return (ms_restore_signals(shell), close(wfd), unlink(path), free(path), -1);
	while (waitpid(pid, &st, 0) < 0 && errno == EINTR)
		;
	ms_restore_signals(shell);
	close(wfd);
	if (WIFSIGNALED(st) && WTERMSIG(st) == SIGINT)
		return (unlink(path), free(path), -2);
	if (WIFEXITED(st))
		*lines_read = WEXITSTATUS(st);
	rfd = open(path, O_RDONLY | O_CLOEXEC);
	unlink(path);
	free(path);
	if (rfd < 0)
		return (-1);
	return (rfd);
}

static void	ms_close_all_heredocs(t_command *cmds)
{
	t_command	*c;
	t_redir		*r;

	c = cmds;
	while (c)
	{
		r = c->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC && r->heredoc_fd >= 0)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		c = c->next;
	}
}

int	ms_prepare_heredocs(t_shell *shell, t_command *cmds)
{
	t_command	*c;
	t_redir		*r;
	int			fd;
	int			lines_read;
	int			hd_line_num;

	hd_line_num = 1;
	c = cmds;
	while (c)
	{
		r = c->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC)
			{
				r->heredoc_line = hd_line_num;
				fd = ms_build_one_heredoc(shell, cmds, r, &lines_read);
				if (fd == -2)
				{
					g_signal_number = SIGINT;
					ms_close_all_heredocs(cmds);
					return (130);
				}
				if (fd < 0)
				{
					perror("heredoc");
					ms_close_all_heredocs(cmds);
					return (1);
				}
				r->heredoc_fd = fd;
				hd_line_num += lines_read;
				shell->input_line_num++;
			}
			r = r->next;
		}
		c = c->next;
	}
	return (0);
}
