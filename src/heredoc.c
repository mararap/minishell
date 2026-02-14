/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 11:37:31 by jatanaso          #+#    #+#             */
/*   Updated: 2026/02/14 13:59:57 by marapovi         ###   ########.fr       */
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
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0600);
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

static int	ms_hd_child_loop(t_shell *shell, t_redir *r, int wfd)
{
	char	*line;
	char	*out;

	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
			break ;
		if (ft_strcmp(line, r->target) == 0)
		//if (ft_strncmp(line, r->target, ft_strlen(r->target) + 1) == 0)
		{
			free(line);
			break ;
		}
		if (r->heredoc_expand)
		{
			out = ms_expand_heredoc_line(shell, line);
			free(line);
			if (!out)
				return (1);
			write(wfd, out, ft_strlen(out));
			write(wfd, "\n", 1);
			free(out);
		}
		else
		{
			write(wfd, line, ft_strlen(line));
			write(wfd, "\n", 1);
			free(line);
		}
	}
	return (0);
}

/* returns: fd >= 0 success, -2 SIGINT, -1 error */
static int	ms_build_one_heredoc(t_shell *shell, t_redir *r)
{
	char	*path;
	int		wfd;
	int		rfd;
	pid_t	pid;
	int		st;

	path = NULL;
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
	rfd = open(path, O_RDONLY);
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

	c = cmds;
	while (c)
	{
		r = c->redirections;
		while (r)
		{
			if (r->type == REDIR_HEREDOC)
			{
				fd = ms_build_one_heredoc(shell, r);
				if (fd == -2)
				{
					g_signal_number = SIGINT;
					ms_close_all_heredocs(cmds);
					return (130);
				}
				if (fd < 0)
				{
					ms_perror("heredoc");
					ms_close_all_heredocs(cmds);
					return (1);
				}
				r->heredoc_fd = fd;
			}
			r = r->next;
		}
		c = c->next;
	}
	return (0);
}
