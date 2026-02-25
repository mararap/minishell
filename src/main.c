/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 20:32:50 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/25 13:27:33 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_number = 0;

static void	ms_detect_interactive(t_shell *shell)
{
	if (isatty(STDIN_FILENO))
		// system function to check if STDIN is "at tty" aka connected to a terminal
		shell->is_interactive = 1;
	else
		shell->is_interactive = 0;
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	// Maria to check if this is needed
	(void)argc;
	(void)argv;
	ms_init_shell(&shell, envp);
	ms_detect_interactive(&shell);
	ms_main_loop(&shell);
	ms_free_shell(&shell);
	return (shell.last_exit_status);
}
// main for debugger
/* int main(int ac, char*av[], char **envp)
{
	if (ac <= 1)
	{
		write(2, "Wrong test input\n", 17);
		return (1);
	}
	t_shell	shell;
	ms_init_shell(&shell, envp);
		char	*line;
	int i = 1;
	while (i < ac - 1)
	{
		// if (shell.is_interactive)
		// 	ms_setup_interactive_signals();
		line = av[i];
		if (!line)
		{
			if (shell.is_interactive)
				write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (line[0] != '\0')
			add_history(line);
		ms_handle_line(&shell, line);
		i++;
	}
	ms_free_shell(&shell);
	return (shell.last_exit_status);
} */
