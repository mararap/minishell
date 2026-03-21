/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_gnl.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_chomp_eol(char *s)
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

static ssize_t	ms_gnl_read_char(int fd, char *c)
{
	ssize_t	ret;

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
		{
			free(line);
			return (NULL);
		}
		if (ret == 0)
			break ;
		line = ms_gnl_append_char(line, c);
		if (!line || c == '\n')
			break ;
	}
	if (*line == '\0')
	{
		free(line);
		return (NULL);
	}
	return (line);
}