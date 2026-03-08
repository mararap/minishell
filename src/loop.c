/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 21:03:30 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/25 13:30:54 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//Maria
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
	{
		shell->last_exit_status = 2;
		return ;
	}
	status = ms_prepare_heredocs(shell, commands);
	if (status != 0)
	{
		shell->last_exit_status = status;
		ms_free_command_list(commands);
		return ;
	}
	status = ms_execute_pipeline(shell, commands);
	shell->last_exit_status = status;
	ms_free_command_list(commands);
}
/*
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 42
#endif

static char	*get_next_line(int fd)
{
	int bytes_read = 0;
	int i = 0;
	char c;
	if(fd < 0 || BUFFER_SIZE < 1)
		  return NULL;	

	char *line = malloc(100000);
	if(!line)
		return NULL;
		
	while((bytes_read = read(fd, &c, 1)) > 0)
	{
		line[i++] = c;
		if(c == '\n')
			break ;
	}
	if (bytes_read < 0 ||  i == 0)
		return (free(line), NULL);
	
	line[i] = '\0';

	char *new_line = malloc(i + 1);
	if (!new_line)
		return (free(line), NULL);
	i = 0;

	while(line[i])
	{
		new_line[i] = line[i];
		i++;
	}
	new_line[i] = '\0';
	free(line);

	return new_line;	
}*/
//Tester LOOP Version
void	ms_main_loop(t_shell *shell)
{
	char	*line;

	while(1)
	{
		if (shell->is_interactive)
			ms_setup_interactive_signals();
		if (isatty(STDIN_FILENO))
		{
			line = readline(PROMPT_STR);
			if (!line)
			{
				if (isatty(STDIN_FILENO))
					write(STDOUT_FILENO, "exit\n", 5);
				break;
			}
		}
		else
		{
			char *raw;
			raw = ms_get_next_line(STDIN_FILENO);
			if (!raw)
				break;
			line = ft_strtrim(raw, "\r\n");
			free(raw);
		}
		if (isatty(STDIN_FILENO) && line[0] != '\0')
			add_history(line);
		ms_handle_line(shell, line);
		free(line);
	}
	exit(shell->last_exit_status);
}

//Origninal LOOP
/* void	ms_main_loop(t_shell *shell)
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
} */

