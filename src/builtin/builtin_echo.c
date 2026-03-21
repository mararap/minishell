/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:25:19 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/15 21:07:21 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_is_valid_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] == '\0')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static int	ms_echo_skip_flags(char **argv, int *print_newline)
{
	int	i;

	i = 1;
	*print_newline = 1;
	while (argv[i] && ms_is_valid_n_flag(argv[i]))
	{
		*print_newline = 0;
		i++;
	}
	return (i);
}

static int	ms_echo_print_args(char **argv, int start)
{
	int	first;

	first = 1;
	while (argv[start])
	{
		if (!first && write(STDOUT_FILENO, " ", 1) < 0)
			return (1);
		if (write(STDOUT_FILENO, argv[start], ft_strlen(argv[start])) < 0)
			return (1);
		first = 0;
		start++;
	}
	return (0);
}

static int	ms_ignore_sigpipe(struct sigaction *old_action)
{
	struct sigaction	new_action;

	ft_bzero(&new_action, sizeof(new_action));
	new_action.sa_handler = SIG_IGN;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	if (sigaction(SIGPIPE, &new_action, old_action) < 0)
		return (1);
	return (0);
}

int	ms_builtin_echo(char **argv)
{
	struct sigaction	old_action;
	int					start;
	int					print_newline;
	int					status;

	start = ms_echo_skip_flags(argv, &print_newline);
	if (ms_ignore_sigpipe(&old_action))
		return (1);
	status = ms_echo_print_args(argv, start);
	if (!status && print_newline && write(STDOUT_FILENO, "\n", 1) < 0)
		status = 1;
	sigaction(SIGPIPE, &old_action, NULL);
	return (status);
}
