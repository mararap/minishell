/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 20:32:50 by marapovi          #+#    #+#             */
/*   Updated: 2026/01/12 16:49:07 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_number = 0;

static void	ms_detect_interactive(t_shell *shell)
{
	if (isatty(STDIN_FILENO))
		//system function to check if STDIN is "at tty"/connected to a terminal
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
	ms_main_loop(&shell);
	ms_free_shell(&shell);
	return (shell.last_exit_status);
}
