/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 20:32:50 by marapovi          #+#    #+#             */
/*   Updated: 2026/01/11 21:35:06 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_number = 0;

static void	ms_detect_interactive(t_shell *shell)
{
	if (isatty(STDIN_FILENO))
		shell->is_interactive = 1;
	else
		shell->is_interactive = 0;
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;

	(void)ac;
	(void)av;
	ms_init_shell(&shell, envp);
	ms_detect_interactive(&shell);
}
