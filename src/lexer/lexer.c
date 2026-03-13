/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 21:16:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/13 18:42:01 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_add_redir_token(t_token **tokens, t_lexr_state *lstate)
{
	if (lstate->line[lstate->i] == '<' && lstate->line[lstate->i + 1] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_HEREDOC, NULL, 0));
		lstate->i += 2;
		lstate->expect_heredoc_delim = 0;
	}
	else if (lstate->line[lstate->i] == '>'
		&& lstate->line[lstate->i + 1] == '>')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_APPEND, NULL, 0));
		lstate->i += 2;
		lstate->expect_heredoc_delim = 1;
	}
	else if (lstate->line[lstate->i] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_IN, NULL, 0));
		lstate->i += 2;
		lstate->expect_heredoc_delim = 0;
	}
	else
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_OUT, NULL, 0));
		lstate->i += 2;
		lstate->expect_heredoc_delim = 0;
	}
}

static int	ms_lex_word(t_token **tokens, t_lexr_state *lstate)
{
	char	*word;
	int		quoted;
	
	quoted = 0;
	word = ms_collect_word(lstate->shell, lstate->line, &lstate->i, &quoted,
		!lstate->expect_heredoc_delim);
	if (!word)
		return (0);
	ms_token_add_back(tokens, ms_token_new(TOKEN_WORD, word, quoted));
	lstate->expect_heredoc_delim = 0;
	return (1);
}

static int	ms_lex_tokens(t_token **tokens, t_lexr_state *lstate)
{
	if (lstate->line[lstate->i] == '|')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_PIPE, NULL, 0));
		lstate->i++;
		lstate->expect_heredoc_delim = 0;
	}
	else if (lstate->line[lstate->i] == '<' || lstate->line[lstate->i] == '>')
		ms_add_redir_token(tokens, lstate);
	else if (!ms_lex_word(tokens, lstate))
		return(0);
	return (1);
}

t_token	*ms_lex_line(t_shell *shell, char *line)
{
	t_token			*tokens;
	t_lexr_state	lstate;

	tokens = NULL;
	lstate.shell = shell;
	lstate.line = line;
	lstate.i = 0;
	lstate.expect_heredoc_delim = 0;
	while (lstate.line[lstate.i])
	{
		while (ft_isspace(lstate.line[lstate.i]))
			lstate.i++;
		if(!lstate.line || lstate.line[lstate.i] == '#')
			break ;
		if(!ms_lex_tokens(&tokens, &lstate))
			break ;
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
