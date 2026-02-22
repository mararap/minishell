#include "minishell.h"

void	*ms_xmalloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
	{
		write(STDERR_FILENO, SHELL_NAME ": malloc failed\n", 24);
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
	int	exit_code;

	exit_code = 127;
	write (STDERR_FILENO, arg, ft_strlen(arg));
	write (STDERR_FILENO, ": ", 2);
	if (err_no == ENOENT)
		write (STDERR_FILENO, "No such file or directory\n", 26);
	else if (err_no == EISDIR || err_no == ENOEXEC)
	{
		if (err_no == EISDIR)
			write (STDERR_FILENO, "Is a directory\n", 15);
		else
			write(STDERR_FILENO, "Exec format error\n", 18);
		exit_code = 126;
	}
	else
	{
		write(STDERR_FILENO, strerror(err_no), ft_strlen(strerror(err_no)));
		write (STDERR_FILENO, "\n", 1);
	}
	exit (exit_code);
}

void ms_print_command_not_found(char *cmd)
{
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
    write(STDERR_FILENO, ": command not found\n", 20);
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

	ret = read(fd, c, 1);
	if (ret <= 0)
		return (ret);
	return (1);
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


