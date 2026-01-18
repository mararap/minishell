/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 21:03:30 by marapovi          #+#    #+#             */
/*   Updated: 2026/01/18 21:52:17 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_handle_line(t_shell *shell, char *line)
{
	t_token		*tokens;
	t_command	*commands;
	int			status;

	if (!line || line[0] == '\0')
		return ;
	tokens = ms_lex_line(shell, line);
	if (!tokens)
		return ;
	commands = ms_parse_tokens(tokens);
	ms_free_token_list(tokens);
	if (!commands)
		return ;
	status = ms_execute_pipeline(shell, commands);
	shell->last_exit_status = status;
	ms_free_command_list(commands);
}

void	ms_main_loop(t_shell *shell)
{
	char *line;

	while(1)
	{
		if (shell->is_interactive)
			ms_setup_interactive_signals();
		line = readline(PROMPT_STR);
		if (!line)
		{
			if (shell->is_interactive)
				write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		if (line[0] != '\0')
			add_history(line);
		ms_handle_line(shell, line);
		free(line);
	}
}

