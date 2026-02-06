/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 21:16:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/06 15:27:17 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_add_redir_token(t_token **tokens, char *str, int *idx)
{
	if (str[*idx + 1] && str[*idx] == '<' && str[*idx + 1] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_HEREDOC, NULL));
		(*idx) += 2;
	}
	else if (str[*idx] == '>' && str[*idx + 1] == '>')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_APPEND, NULL));
		(*idx) += 2;
	}
	else if (str[*idx] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_IN, NULL));
		(*idx)++;
	}
	else
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_OUT, NULL));
		(*idx)++;
	}
}

static void	ms_lex_tokens(t_token **tokens, char *line, int *i)
{
	if (line[*i] == '|')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_PIPE, NULL));
		(*i)++;
	}
	else if (line[*i] == '<' || line[*i] == '>')
		ms_add_redir_token(tokens, line, i);
}

//Maria to check for memory leaks
t_token	*ms_lex_line(t_shell *shell, char *line)
{
	t_token	*tokens;
	char	*word;
	int		i;

	tokens = NULL;
	i = 0;
	while (line[i])
	{
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (!line[i])
			break ;
		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
			ms_lex_tokens(&tokens, line, &i);
		else
		{
			word = ms_collect_word(shell, line, &i);
			if (!word)
				break ;
			ms_token_add_back(&tokens, ms_token_new(TOKEN_WORD, word));
		}
	}
	return (tokens);
}

void	ms_free_token_list(t_token *token_list)
{
	t_token	*next;

	while (token_list)
	{
		next = token_list->next;
		if (token_list->value)
			free(token_list->value);
		free(token_list);
		token_list = next;
	}
}
