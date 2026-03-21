/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_perror.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_exec_exit_code(int err_no)
{
	if (err_no == EISDIR || err_no == ENOEXEC || err_no == EACCES)
		return (126);
	return (127);
}

static char	*ms_exec_error_message(int err_no)
{
	if (err_no == ENOENT)
		return ("No such file or directory");
	if (err_no == EISDIR)
		return ("Is a directory");
	if (err_no == ENOEXEC)
		return ("Exec format error");
	if (err_no == EACCES)
		return ("Permission denied");
	return ((char *)strerror(err_no));
}

void	ms_perror(char *arg, int err_no)
{
	char	*line;
	char	*msg;

	if (err_no == 0)
		err_no = ENOENT;
	msg = ms_str_join_three(arg, ": ", ms_exec_error_message(err_no));
	if (!msg)
		exit(ms_exec_exit_code(err_no));
	line = ms_str_join_three(msg, "\n", "");
	free(msg);
	if (!line)
		exit(ms_exec_exit_code(err_no));
	write(STDERR_FILENO, line, ft_strlen(line));
	free(line);
	exit(ms_exec_exit_code(err_no));
}