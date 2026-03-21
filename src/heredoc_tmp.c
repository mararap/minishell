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
