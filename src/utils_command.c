/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static int	ms_diag_named_escape(unsigned char c, char *dst)
{
	if (c == '\a')
		dst[1] = 'a';
	else if (c == '\b')
		dst[1] = 'b';
	else if (c == '\t')
		dst[1] = 't';
	else if (c == '\n')
		dst[1] = 'n';
	else if (c == '\v')
		dst[1] = 'v';
	else if (c == '\f')
		dst[1] = 'f';
	else if (c == '\r')
		dst[1] = 'r';
	else
		return (0);
	dst[0] = '\\';
	return (1);
}

static size_t	ms_diag_put_escape(char *dst, unsigned char c)
{
	if (ms_diag_named_escape(c, dst))
		return (2);
	if (c == '\\' || c == '\'')
	{
		dst[0] = '\\';
		dst[1] = c;
		return (2);
	}
	if (!ft_isprint(c))
	{
		dst[0] = '\\';
		dst[1] = '0' + ((c >> 6) & 7);
		dst[2] = '0' + ((c >> 3) & 7);
		dst[3] = '0' + (c & 7);
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

	if (!cmd)
		return (ft_strdup(""));
	if (!ms_needs_ansi_quote(cmd))
		return (ft_strdup(cmd));
	out = (char *)ft_calloc((ft_strlen(cmd) * 4) + 4, sizeof(char));
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	out[j++] = '$';
	out[j++] = '\'';
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