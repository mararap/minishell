#include "minishell.h"

void	*ms_xmalloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		write(STDERR_FILENO, SHELL_NAME ": malloc failed\n",
			ft_strlen(SHELL_NAME) + 16);
		exit(1);
	}
	return (ptr);
}

char	*ms_strdup_safe(const char *src)
{
	if (!src)
		return (NULL);
	return (ft_strdup(src));
}

void	ms_perror(char *arg, int err_no)
{
	int		exit_code;
	char	*msg;
	char	*line;
	char	*prefix;

	exit_code = 127;
	if (err_no == EISDIR || err_no == ENOEXEC || err_no == EACCES)
		exit_code = 126;
	if (err_no == ENOENT)
		msg = "No such file or directory";
	else if (err_no == EISDIR)
		msg = "Is a directory";
	else if (err_no == ENOEXEC)
		msg = "Exec format error";
	else if (err_no == EACCES)
		msg = "Permission denied";
	else
		msg = (char *)strerror(err_no);
	prefix = ms_str_join_three(arg, ": ", "");
	if (!prefix)
		exit(exit_code);
	line = ms_str_join_three(prefix, msg, "\n");
	free(prefix);
	if (!line)
		exit(exit_code);
	write(STDERR_FILENO, line, ft_strlen(line));
	free(line);
	exit(exit_code);
}

static int	ms_needs_ansi_quote(const char *s)
{
	while (*s)
	{
		if (!ft_isprint((unsigned char)*s))
			return (1);
		s++;
	}
	return (0);
}

static size_t	ms_diag_char_len(unsigned char c)
{
	if (c == '\a' || c == '\b' || c == '\t' || c == '\n')
		return (2);
	if (c == '\v' || c == '\f' || c == '\r' || c == '\\')
		return (2);
	if (c == '\'')
		return (2);
	if (!ft_isprint(c))
		return (4);
	return (1);
}

static size_t	ms_diag_put_escape(char *dst, unsigned char c)
{
	if (c == '\a')
		return (dst[0] = '\\', dst[1] = 'a', 2);
	if (c == '\b')
		return (dst[0] = '\\', dst[1] = 'b', 2);
	if (c == '\t')
		return (dst[0] = '\\', dst[1] = 't', 2);
	if (c == '\n')
		return (dst[0] = '\\', dst[1] = 'n', 2);
	if (c == '\v')
		return (dst[0] = '\\', dst[1] = 'v', 2);
	if (c == '\f')
		return (dst[0] = '\\', dst[1] = 'f', 2);
	if (c == '\r')
		return (dst[0] = '\\', dst[1] = 'r', 2);
	if (c == '\\' || c == '\'')
		return (dst[0] = '\\', dst[1] = c, 2);
	if (!ft_isprint(c))
	{
		dst[0] = '\\';
		dst[1] = '0' + ((c >> 6) & 7);
		dst[2] = '0' + ((c >> 3) & 7);
		dst[3] = '0' + (c + 7);
		return (4);
	}
	dst[0] = c;
	return (1);
}

static char	*ms_format_cmd_name(const char *cmd)
{
	char	*out;
	size_t	i;
	size_t	j;
	size_t	len;

	if (!cmd)
		return (ft_strdup(""));
	if (!ms_needs_ansi_quote(cmd))
		return (ft_strdup(cmd));
	len = 3;
	i = 0;
	while (cmd[i])
		len += ms_diag_char_len((unsigned char)cmd[i++]);
	out = (char *)ft_calloc(len + 1, sizeof(char));
	if (!out)
		return (NULL);
	j = 0;
	out[j++] = '$';
	out[j++] = '\'';
	i = 0;
	while (cmd[i])
		j += ms_diag_put_escape(out + j, (unsigned char)cmd[i++]);
	out[j++] = '\'';
	return (out);
}

void	ms_print_command_not_found(char *cmd)
{
	char	*display;
	char	*msg;

	display = ms_format_cmd_name(cmd);
	if (!display)
		return ;
	msg = ms_str_join_three(display, ": command not found", "\n");
	free(display);
	if (!msg)
		return ;
	write(STDERR_FILENO, msg, ft_strlen(msg));
	free(msg);
}

size_t	ms_str_arr_len(char **arr)
{
	size_t	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

void	ms_free_str_array(char **arr)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

char	*ms_str_join_three(char *a, char *b, char *c)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(a, b);
	if (!tmp)
		return (NULL);
	res = ft_strjoin(tmp, c);
	free(tmp);
	return (res);
}

static ssize_t	ms_gnl_read_char(int fd, char *c)
{
	ssize_t	ret;

	/* 	ret = read(fd, c, 1);
		if (ret <= 0)
			return (ret);
		return (1); */
	while (1)
	{
		ret = read(fd, c, 1);
		if (ret < 0 && (errno == EINTR || errno == EAGAIN
				|| errno == EWOULDBLOCK))
			continue ;
		return (ret);
	}
}

static char	*ms_gnl_append_char(char *line, char c)
{
	char	buf[2];
	char	*tmp;

	buf[0] = c;
	buf[1] = '\0';
	tmp = line;
	line = ft_strjoin(tmp, buf);
	free(tmp);
	return (line);
}

char	*ms_get_next_line(int fd)
{
	char	*line;
	char	c;
	ssize_t	ret;

	if (fd < 0)
		return (NULL);
	line = ft_strdup("");
	if (!line)
		return (NULL);
	ret = 1;
	while (ret > 0)
	{
		ret = ms_gnl_read_char(fd, &c);
		if (ret < 0)
			return (free(line), NULL);
		if (ret == 0)
			break ;
		line = ms_gnl_append_char(line, c);
		if (!line || c == '\n')
			break ;
	}
	if (*line == '\0')
		return (free(line), NULL);
	return (line);
}

/*“We separated reading, memory concatenation, and control
flow into independent functions. This keeps each function
under 25 lines, avoids static state, and makes heredoc behavior
predictable and signal-safe.”*/
