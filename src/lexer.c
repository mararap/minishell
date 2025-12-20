#include "minishell.h"

static t_token	*ms_token_new(t_token_type type, char *value)
{
	t_token	*tok;

	tok = (t_token *)ms_xmalloc(sizeof(t_token));
	tok->type = type;
	tok->value = value;
	tok->next = NULL;
	return (tok);
}

static void	ms_token_add_back(t_token **list, t_token *new_tok)
{
	t_token	*iter;

	if (!*list)
	{
		*list = new_tok;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_tok;
}

static char	*ms_expand_variable(t_shell *shell, char *str, int *idx)
{
	int		start;
	char	*name;
	char	*value;

	if (str[*idx + 1] == '?')
	{
		*idx = *idx + 2;
		return (ft_itoa(shell->last_exit_status));
	}
	start = *idx + 1;
	while (str[start] && (ft_isalnum(str[start]) || str[start] == '_'))
		start++;
	name = ft_substr(str, *idx + 1, start - (*idx + 1));
	value = ms_env_get_value(shell->env_list, name);
	free(name);
	*idx = start;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}

static char	*ms_collect_word(t_shell *shell, char *str, int *idx)
{
	char	*buf;
	char	*tmp;
	char	ch;
	int		start;

	buf = ft_strdup("");
	while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
		&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>')
	{
		if (str[*idx] == '\'')
		{
			*idx = *idx + 1;
			start = *idx;
			while (str[*idx] && str[*idx] != '\'')
				*idx = *idx + 1;
			if (!str[*idx])
				return (buf);
			tmp = ft_substr(str, start, *idx - start);
			ch = str[*idx];
			*idx = *idx + 1;
		}
		else if (str[*idx] == '"')
		{
			*idx = *idx + 1;
			start = *idx;
			while (str[*idx] && str[*idx] != '"')
				*idx = *idx + 1;
			tmp = ft_substr(str, start, *idx - start);
			if (str[*idx] == '"')
				*idx = *idx + 1;
		}
		else if (str[*idx] == '$')
		{
			tmp = ms_expand_variable(shell, str, idx);
		}
		else
		{
			start = *idx;
			while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
				&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>'
				&& str[*idx] != '\'' && str[*idx] != '"' && str[*idx] != '$')
				*idx = *idx + 1;
			tmp = ft_substr(str, start, *idx - start);
		}
		buf = ft_strjoin(buf, tmp);
		free(tmp);
	}
	return (buf);
}

static void	ms_add_redir_token(t_token **tokens, char *str, int *idx)
{
	if (str[*idx] == '<' && str[*idx + 1] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_HEREDOC, NULL));
		*idx = *idx + 2;
	}
	else if (str[*idx] == '>' && str[*idx + 1] == '>')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_APPEND, NULL));
		*idx = *idx + 2;
	}
	else if (str[*idx] == '<')
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_IN, NULL));
		*idx = *idx + 1;
	}
	else
	{
		ms_token_add_back(tokens, ms_token_new(TOKEN_REDIR_OUT, NULL));
		*idx = *idx + 1;
	}
}

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
		if (line[i] == '|')
		{
			ms_token_add_back(&tokens, ms_token_new(TOKEN_PIPE, NULL));
			i++;
		}
		else if (line[i] == '<' || line[i] == '>')
			ms_add_redir_token(&tokens, line, &i);
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
