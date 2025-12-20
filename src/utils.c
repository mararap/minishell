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

void	ms_perror(char *msg)
{
	perror(msg);
}

void	ms_print_command_not_found(char *cmd)
{
	write(STDERR_FILENO, SHELL_NAME ": ", 11);
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, ": command not found\n", 21);
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
