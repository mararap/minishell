/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:40 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/15 15:24:42 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_exit_too_many(void)
{
	write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
	write(STDERR_FILENO, ": exit: ", 8);
	write(STDERR_FILENO, "too many arguments\n", 19);
	return (1);
}

static int	ms_exit_numeric_error(char *arg)
{
	write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
	write(STDERR_FILENO, ": exit: ", 8);
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": numeric argument required\n", 28);
	return (2);
}

static int	ms_mark_exit(t_shell *shell, int status)
{
	shell->should_exit = 1;
	return (status);
}

int	ms_builtin_exit(t_shell *shell, char **argv)
{
	long long	value;

	if (shell->is_interactive && getpid() == shell->main_pid)
		write(STDOUT_FILENO, "exit\n", 5);
	if (!argv[1])
		return (ms_mark_exit(shell, shell->last_exit_status));
	if (ft_strncmp(argv[1], "--", 3) == 0)
	{
		if (argv[2])
			return (ms_exit_too_many());
		return (ms_mark_exit(shell, shell->last_exit_status));
	}
	if (argv[1][0] == '\0' || !ms_atoll_strict(argv[1], &value))
		return (ms_mark_exit(shell, ms_exit_numeric_error(argv[1])));
	if (argv[2])
		return (ms_exit_too_many());
	return (ms_mark_exit(shell, (unsigned char)value));
}
