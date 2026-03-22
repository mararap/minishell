/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_tmp.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_restore_signals(t_shell *shell)
{
	if (shell->is_interactive)
		ms_setup_interactive_signals();
	else
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
}

static char	*ms_hd_make_path(int n)
{
	char	*num;
	char	*path;

	num = ft_itoa(n);
	if (!num)
		return (NULL);
	path = ms_str_join_three("/tmp/minishell_hd_", num, "");
	free(num);
	return (path);
}

int	ms_hd_open_tmp(char **out_path)
{
	static int	counter = 0;
	char		*path;
	int			fd;

	while (counter < 1000000)
	{
		path = ms_hd_make_path(counter++);
		if (!path)
			return (-1);
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0600);
		if (fd >= 0)
		{
			*out_path = path;
			return (fd);
		}
		free(path);
		if (errno != EEXIST)
			return (-1);
	}
	return (-1);
}

static void	ms_sigint_heredoc(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	rl_done = 1;
	write(STDOUT_FILENO, "\n", 1);
}

void	ms_setup_heredoc_child_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_bzero(&sa_int, sizeof(sa_int));
	ft_bzero(&sa_quit, sizeof(sa_quit));
	sa_int.sa_handler = ms_sigint_heredoc;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}
