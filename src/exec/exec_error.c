/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
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

static void	ms_write_exec_message(int err_no)
{
	if (err_no == ENOENT)
		write(STDERR_FILENO, "No such file or directory\n", 26);
	else if (err_no == EISDIR)
		write(STDERR_FILENO, "Is a directory\n", 15);
	else if (err_no == ENOEXEC)
		write(STDERR_FILENO, "Exec format error\n", 18);
	else if (err_no == EACCES)
		write(STDERR_FILENO, "Permission denied\n", 18);
	else
	{
		write(STDERR_FILENO, strerror(err_no), ft_strlen(strerror(err_no)));
		write(STDERR_FILENO, "\n", 1);
	}
}

int	ms_exec_error_code(char *arg, int err_no)
{
	if (err_no == 0)
		err_no = ENOENT;
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": ", 2);
	ms_write_exec_message(err_no);
	return (ms_exec_exit_code(err_no));
}

static int	ms_exec_directory_status(char *argv0, char *display_arg)
{
	if (ft_strcmp(argv0, ".") == 0)
	{
		ms_print_command_not_found(argv0);
		return (127);
	}
	write(STDERR_FILENO, display_arg, ft_strlen(display_arg));
	write(STDERR_FILENO, ": Is a directory\n", 17);
	return (126);
}

int	ms_exec_precheck(char *argv0, char *display_arg, char *path)
{
	struct stat	file_info;

	if (stat(path, &file_info) == -1)
		return (ms_exec_error_code(display_arg, errno));
	if (S_ISDIR(file_info.st_mode))
		return (ms_exec_directory_status(argv0, display_arg));
	return (-1);
}
