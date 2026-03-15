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

typedef struct s_hd_buf
{
	char	*buf;
	size_t	len;
	size_t	cap;
}			t_hd_buf;

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
		path = ms_hd_make_path(counter++);
		if (!path)
			return (-1);
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0600);
		if (fd >= 0)
			return (*out_path = path, fd);
		free(path);
		if (errno != EEXIST)
			return (-1);
	}
	return (-1);
}

static int	ms_hd_init_buf(t_hd_buf *b)
{
	b->cap = 64;
	b->len = 0;
	b->buf = (char *)malloc(b->cap);
	if (!b->buf)
		return (-1);
	b->buf[0] = '\0';
	return (0);
}

static int	ms_hd_grow_buf(t_hd_buf *b)
{
	char	*newbuf;
	size_t	i;

	if (b->len + 2 <= b->cap)
		return (0);
	b->cap = (b->cap * 2) + 64;
	newbuf = (char *)malloc(b->cap);
	if (!newbuf)
		return (-1);
	i = 0;
	while (i < b->len)
	{
		newbuf[i] = b->buf[i];
		i++;
	}
	free(b->buf);
	b->buf = newbuf;
	return (0);
}

static int	ms_hd_append_char(t_hd_buf *b, char c)
{
	if (ms_hd_grow_buf(b) < 0)
		return (-1);
	b->buf[b->len++] = c;
	b->buf[b->len] = '\0';
	return (0);
}

static int	ms_hd_append_str(t_hd_buf *b, char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (ms_hd_append_char(b, s[i]) < 0)
			return (-1);
		i++;
	}
	return (0);
}

static int	ms_hd_append_status(t_shell *shell, t_hd_buf *b, size_t *i)
{
	char	*tmp;
	int		status;

	tmp = ft_itoa(shell->last_exit_status);
	if (!tmp)
		return (-1);
	status = ms_hd_append_str(b, tmp);
	free(tmp);
	*i += 2;
	return (status);
}
static int	ms_hd_append_var(t_shell *shell, char *line, t_hd_buf *b, size_t *i)
{
	size_t	j;
	char	*name;
	char	*val;

	j = *i + 1;
	while (line[j] && (ft_isalnum(line[j]) || line[j] == '_'))
		j++;
	name = ft_substr(line, *i + 1, j - (*i + 1));
	if (!name)
		return (-1);
	val = ms_env_get_value(shell->env_list, name);
	free(name);
	*i = j;
	return (ms_hd_append_str(b, val));
}
static int	ms_hd_expand_step(t_shell *shell, char *line, t_hd_buf *b,
		size_t *i)
{
	if (line[*i] == '\\' && line[*i + 1] == '$')
	{
		*i += 2;
		return (ms_hd_append_char(b, '$'));
	}
	if (line[*i] == '$' && line[*i + 1] == '?')
		return (ms_hd_append_status(shell, b, i));
	if (line[*i] == '$' && (ft_isalnum(line[*i + 1]) || line[*i + 1] == '_'))
		return (ms_hd_append_var(shell, line, b, i));
	return (ms_hd_append_char(b, line[(*i)++]));
}
static char	*ms_expand_heredoc_line(t_shell *shell, char *line)
{
	t_hd_buf	b;
	size_t		i;

	if (ms_hd_init_buf(&b) < 0)
		return (NULL);
	i = 0;
	while (line[i])
	{
		if (ms_hd_expand_step(shell, line, &b, &i) < 0)
			return (free(b.buf), NULL);
	}
	return (b.buf);
}
static void	ms_hd_chomp_eol(char *s)
{
	size_t	len;

	if (!s)
		return ;
	len = ft_strlen(s);
	while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
	{
		s[len - 1] = '\0';
		len--;
	}
}

static char	*ms_hd_read_line(t_shell *shell)
{
	char	*line;

	if (shell->is_interactive)
		return (readline(HEREDOC_PROMPT));
	line = ms_get_next_line(STDIN_FILENO);
	if (line)
		ms_hd_chomp_eol(line);
	return (line);
}
static void	ms_hd_warn_eof(t_redir *r)
{
	char	*line_num_str;

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
}

static int	ms_hd_write_line(t_shell *shell, t_redir *r, int wfd, char *line)
{
	char	*expanded;

	if (!r->heredoc_expand)
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

static int	ms_hd_child_loop(t_shell *shell, t_redir *r, int wfd)
{
	char	*line;
	int		cur_line;

	cur_line = r->heredoc_line;
	while (1)
	{
		line = ms_hd_read_line(shell);
		cur_line++;
		if (!line)
			return (ms_hd_warn_eof(r), cur_line - r->heredoc_line);
		if (ft_strcmp(line, r->target) == 0)
			return (free(line), cur_line - r->heredoc_line);
		if (ms_hd_write_line(shell, r, wfd, line) != 0)
			return (1);
	}
}
static void	ms_hd_child_exit(t_shell *shell, t_command *cmds, char *path,
		int wfd)
{
	close(wfd);
	if (shell->current_line)
	{
		free(shell->current_line);
		shell->current_line = NULL;
 	}
	ms_free_command_list(cmds);
	ms_free_shell(shell);
	free(path);
}

static int	ms_hd_finalize_build(t_shell *shell, char *path, int wfd,
		int st, int *lines_read)
{
	int	rfd;

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

static int	ms_hd_run_child(t_shell *shell, t_command *cmds,
		t_redir *r, char *path, int wfd)
{
	int	st;

	ms_setup_child_signals();
	st = ms_hd_child_loop(shell, r, wfd);
	ms_hd_child_exit(shell, cmds, path, wfd);
	exit(st);
}

static int	ms_hd_wait_child(pid_t pid, int *st)
{
	if (pid < 0)
		return (-1);
	while (waitpid(pid, st, 0) < 0 && errno == EINTR)
		;
	return (0);
}

/* returns: fd >= 0 success, -2 SIGINT, -1 error */
static int	ms_build_one_heredoc(t_shell *shell, t_command *cmds, t_redir *r,
		int *lines_read)
{
	char	*path;
	int		wfd;
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
		ms_hd_run_child(shell, cmds, r, path, wfd);
	if (ms_hd_wait_child(pid, &st) < 0)
		return (ms_restore_signals(shell), close(wfd), unlink(path), free(path),
			-1);
	return (ms_hd_finalize_build(shell, path, wfd, st, lines_read));
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

static int	ms_prepare_one_heredoc(t_shell *shell, t_command *cmds,
		t_redir *r, int *hd_line_num)
{
	int	fd;
	int	lines_read;

	r->heredoc_line = *hd_line_num;
	fd = ms_build_one_heredoc(shell, cmds, r, &lines_read);
	if (fd == -2)
		return (130);
	if (fd < 0)
		return (1);
	r->heredoc_fd = fd;
	*hd_line_num += lines_read;
	shell->input_line_num++;
	return (0);
}

static int	ms_prepare_command_heredocs(t_shell *shell, t_command *cmds,
		t_command *c, int *hd_line_num)
{
	t_redir	*r;
	int		status;

	r = c->redirections;
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			status = ms_prepare_one_heredoc(shell, cmds, r, hd_line_num);
			if (status != 0)
				return (status);
		}
		r = r->next;
	}
	return (0);
}

int	ms_prepare_heredocs(t_shell *shell, t_command *cmds)
{
	t_command	*c;
	int			hd_line_num;
	int			status;

	hd_line_num = 1;
	c = cmds;
	while (c)
	{
		status = ms_prepare_command_heredocs(shell, cmds, c, &hd_line_num);
		if (status == 130)
			g_signal_number = SIGINT;
		if (status != 0)
		{
			if (status == 1)
				perror("heredoc");
			ms_close_all_heredocs(cmds);
			return (status);
		}
		c = c->next;
	}
	return (0);
}
