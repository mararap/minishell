/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 20:32:50 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/22 17:23:41 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
//test
volatile sig_atomic_t	g_signal_number = 0;

static void	ms_detect_interactive(t_shell *shell)
{
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
		shell->is_interactive = 1;
	else
		shell->is_interactive = 0;
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	ms_init_shell(&shell, envp);
	ms_detect_interactive(&shell);
	ms_main_loop(&shell);
	ms_free_shell(&shell);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	return (shell.last_exit_status);
}
