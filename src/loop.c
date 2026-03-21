/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 21:03:30 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 14:09:28 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_command	*ms_parse_line(t_shell *shell, char *line)
{
	t_token		*tokens;
	t_command	*commands;

	if (!line || line[0] == '\0')
		return (NULL);
	tokens = ms_lex_line(shell, line);
	if (!tokens)
		return (NULL);
	commands = ms_parse_tokens(tokens);
	if (!commands)
	{
		if (!shell->is_interactive)
			ms_discard_pending_heredocs(tokens);
		ms_free_token_list(tokens);
		shell->last_exit_status = 2;
		if (!shell->is_interactive)
			shell->should_exit = 1;
		return (NULL);
	}
	ms_free_token_list(tokens);
	return (commands);
}

static void	ms_execute_commands(t_shell *shell, t_command *commands)
{
	int status;

	status = ms_prepare_heredocs(shell, commands);
	if (status == 0)
		status = ms_execute_pipeline(shell, commands);
	shell->last_exit_status = status;
	ms_free_command_list(commands);
}

static char	*ms_read_line(t_shell *shell)
{
	char	*line;
	if (shell->is_interactive)
	{	
		line = readline(PROMPT_STR);
		if (!line)
			write(STDOUT_FILENO, "exit\n", 5);
		return (line);
	}
	line = ms_get_next_line(STDIN_FILENO);
	if (line)
		ms_chomp_eol(line);
	return (line);
}

void	ms_handle_line(t_shell *shell, char *line)
{
	t_command	*commands;

	commands = ms_parse_line(shell, line);
	if (!commands)
		return ;
	ms_execute_commands(shell, commands);
}

void	ms_main_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		if (shell->is_interactive)
			ms_setup_interactive_signals();
		line = ms_read_line(shell);
		if (!line)
			break ;
		if (shell->is_interactive && line[0] != '\0')
			add_history(line);
		shell->current_line = line;
		ms_handle_line(shell, line);
		free(line);
		shell->current_line = NULL;
		shell->input_line_num++;
	}
}