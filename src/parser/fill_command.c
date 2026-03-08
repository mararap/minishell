/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 20:53:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/01/25 21:26:11 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

# define MS_MASK_SPACE 0x1F
# define MS_MASK_TAB 0x1E
# define MS_MASK_NL 0x1D

static void ms_unmask_ifs(char *s)
{
	if (!s)
		return ;
	while (*s)
	{
		if (*s == ' ')
			*s = MS_MASK_SPACE;
		else if (*s == '\t')
			*s = MS_MASK_TAB;
		else if (*s == '\n')
			*s = MS_MASK_NL;
		s++;
	}
}

static int ms_is_ifs_delim(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static size_t ms_count_ifs_fields(const char *s)
{
	size_t i = 0;
	size_t count = 0;

	while (s[i])
	{
		while (s[i] && ms_is_ifs_delim(s[i]))
			i++;
		if (!s[i])
			break;
		count++;
		while (s[i] && !ms_is_ifs_delim(s[i]))
			i++;
	}
	return count;
}

static char **ms_split_ifs_fields(const char *s)
{
	char **out;
	size_t fields = ms_count_ifs_fields(s);
	size_t i = 0, j = 0, start;

	out = (char **)ms_xmalloc(sizeof(char *) * (fields + 1));
	while (s[i])
	{
		while (s[i] && ms_is_ifs_delim(s[i]))
			i++;
		if (!s[i])
			break;
		start = i;
		while (s[i] && !ms_is_ifs_delim(s[i]))
			i++;
		out[j] = ft_substr(s, start, i - start);
		if (!out)
		{
			write(STDERR_FILENO, SHELL_NAME ": malloc failed\n", 24);
			exit(1);
		}
		ms_unmask_ifs(out[j]);
		j++;
	}
	out[j] = NULL;
	return out;
}

static const char *ms_token_to_str(t_token_type type)
{
	if (type == TOKEN_PIPE)
		return ("|");
	if (type == TOKEN_REDIR_IN)
		return ("<");
	if (type == TOKEN_REDIR_OUT)
		return (">");
	if (type == TOKEN_REDIR_APPEND)
		return (">>");
	if (type == TOKEN_HEREDOC)
		return ("<<");
	return ("newline");
}

static void ms_print_syntax_error(t_token *tok)
{
	const char *unexpected;

	if(!tok)
		unexpected = "newline";
	else
		unexpected = ms_token_to_str(tok->type);
	ft_putstr_fd(SHELL_NAME ": syntax error near unexpected token `", STDERR_FILENO);
	ft_putstr_fd((char *)unexpected, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
}

static t_redir	*ms_create_redir(int type, char *target, int heredoc_expand)
{
	t_redir	*redir;

	redir = (t_redir *)ms_xmalloc(sizeof(t_redir));
	redir->type = type;
	redir->target = target;
	redir->heredoc_fd = -1;
	redir->heredoc_expand = heredoc_expand;
	redir->next = NULL;
	return (redir);
}

static void	ms_redir_add_back(t_redir **list, t_redir *new_redir)
{
	t_redir	*iter;

	if (!*list)
	{
		*list = new_redir;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_redir;
}

static char	**ms_add_word_to_argv(char **argv, char *word)
{
	size_t	len;
	char	**new_argv;
	size_t	i;

	len = ms_str_arr_len(argv);
	new_argv = (char **)ms_xmalloc(sizeof(char *) * (len + 2));
	i = 0;
	while (i < len)
	{
		new_argv[i] = argv[i];
		i++;
	}
	new_argv[len] = word;
	new_argv[len + 1] = NULL;
	free(argv);
	return (new_argv);
}

static void	ms_handle_word(t_command *cmd, t_token **cursor, t_token **tok)
{
	char *word;
	char **parts;
	size_t i;

	word = (*tok)->value;
	if(!word)
	{
		*cursor = (*tok)->next;
		return;
	}
	// Preserve empty word if it came from quotes: echo ""
	if (word[0] == '\0' && (*tok)->quoted)
		cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));
	
	//If there is unmasked IFS whitespace, split into multiple argv entries 
	else if(ft_strchr(word, ' ') || ft_strchr(word, '\t') || ft_strchr(word, '\n'))
	{
		parts = ms_split_ifs_fields(word);

		//if split yields nothing, keeps empty only if there were quotes
		if (!parts[0] && (*tok)->quoted)
			cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));

		i = 0;
		while(parts[i])
		{
			cmd->argv = ms_add_word_to_argv(cmd->argv, parts[i]);
			parts[i] = NULL; 
			i++;
		}
		free(parts);
	}
	else
	{
		//no splitting needed, just unmask and push
		ms_unmask_ifs(word);
		cmd->argv = ms_add_word_to_argv(cmd->argv, word);
		(*tok)->value = NULL;
		*cursor = (*tok)->next;
		return;
	}
	free(word);
	(*tok)->value = NULL;
	*cursor = (*tok)->next;
}

static int	ms_token_to_redir_type(t_token_type t)
{
	if (t == TOKEN_REDIR_IN)
		return (REDIR_IN);
	if (t == TOKEN_REDIR_OUT)
		return (REDIR_OUT);
	if (t == TOKEN_REDIR_APPEND)
		return (REDIR_APPEND);
	if (t == TOKEN_HEREDOC)
		return (REDIR_HEREDOC);
	return (-1);
}

int ms_fill_command(t_command *cmd, t_token **cursor)
{
    t_token *tok;
    int type;

    while (*cursor && (*cursor)->type != TOKEN_PIPE)
    {
        tok = *cursor;
        if (tok->type == TOKEN_WORD)
            ms_handle_word(cmd, cursor, &tok);
        else
        {
            type = ms_token_to_redir_type(tok->type);
			if (type < 0)
			{
				ms_print_syntax_error(tok);
				return (-1);
			}
            *cursor = tok->next;

            if (!*cursor)
            {
                ms_print_syntax_error(NULL);
                return (-1);
            }
			if ((*cursor)->type != TOKEN_WORD)
            {
                ms_print_syntax_error(*cursor);
                return (-1);
            }

            int hd_expand = 0;
            if (type == TOKEN_HEREDOC)
                hd_expand = ((*cursor)->quoted == 0);

			ms_unmask_ifs((*cursor)->value);
            ms_redir_add_back(&cmd->redirections,
                ms_create_redir(type, (*cursor)->value, hd_expand));

            (*cursor)->value = NULL;
            *cursor = (*cursor)->next;
        }
    }
    return (0);
}
