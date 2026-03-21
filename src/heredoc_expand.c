/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expand.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_hd_grow_buf(t_hd_buf *buf, size_t add_len)
{
	char	*newbuf;
	size_t	new_cap;

	if (buf->len + add_len + 1 <= buf->cap)
		return (0);
	new_cap = buf->cap;
	while (buf->len + add_len + 1 > new_cap)
		new_cap = (new_cap * 2) + 64;
	newbuf = (char *)malloc(new_cap);
	if (!newbuf)
		return (-1);
	ft_memcpy(newbuf, buf->buf, buf->len);
	free(buf->buf);
	buf->buf = newbuf;
	buf->cap = new_cap;
	buf->buf[buf->len] = '\0';
	return (0);
}

static int	ms_hd_append_str(t_hd_buf *buf, char *str)
{
	size_t	len;

	if (!str)
		return (0);
	len = ft_strlen(str);
	if (ms_hd_grow_buf(buf, len) < 0)
		return (-1);
	ft_memcpy(buf->buf + buf->len, str, len);
	buf->len += len;
	buf->buf[buf->len] = '\0';
	return (0);
}

static int	ms_hd_append_status_or_var(t_shell *shell, char *line, size_t *i,
		t_hd_buf *buf)
{
	size_t	j;
	char	*name;
	char	*tmp;
	int		status;

	if (line[*i + 1] == '?')
	{
		tmp = ft_itoa(shell->last_exit_status);
		if (!tmp)
			return (-1);
		status = ms_hd_append_str(buf, tmp);
		free(tmp);
		*i += 2;
		return (status);
	}
	j = *i + 1;
	while (line[j] && (ft_isalnum(line[j]) || line[j] == '_'))
		j++;
	name = ft_substr(line, *i + 1, j - (*i + 1));
	if (!name)
		return (-1);
	status = ms_hd_append_str(buf, ms_env_get_value(shell->env_list, name));
	free(name);
	*i = j;
	return (status);
}

static int	ms_hd_append_one(char *line, size_t *i, t_hd_buf *buf)
{
	char	one[2];

	one[1] = '\0';
	if (line[*i] == '\\' && line[*i + 1] == '$')
	{
		one[0] = '$';
		*i += 2;
	}
	else
	{
		one[0] = line[*i];
		(*i)++;
	}
	return (ms_hd_append_str(buf, one));
}

char	*ms_expand_heredoc_line(t_shell *shell, char *line)
{
	t_hd_buf	buf;
	size_t		i;

	buf.cap = 64;
	buf.len = 0;
	buf.buf = (char *)malloc(buf.cap);
	if (!buf.buf)
		return (NULL);
	buf.buf[0] = '\0';
	i = 0;
	while (line[i])
	{
		if (line[i] == '$' && (line[i + 1] == '?'
				|| ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			if (ms_hd_append_status_or_var(shell, line, &i, &buf) < 0)
			{
				free(buf.buf);
				return (NULL);
			}
			continue ;
		}
		if (ms_hd_append_one(line, &i, &buf) < 0)
		{
			free(buf.buf);
			return (NULL);
		}
	}
	return (buf.buf);
}
