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
	cmd->argv = ms_add_word_to_argv(cmd->argv, (*tok)->value);
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
				write(STDERR_FILENO, SHELL_NAME ": syntax error\n", 26);
				return (-1);
			}
            *cursor = tok->next;

            if (!*cursor || (*cursor)->type != TOKEN_WORD)
            {
                write(STDERR_FILENO, SHELL_NAME ": syntax error\n", 26);
                return (-1);
            }

            int hd_expand = 0;
            if (type == TOKEN_HEREDOC)
                hd_expand = ((*cursor)->quoted == 0);

            ms_redir_add_back(&cmd->redirections,
                ms_create_redir(type, (*cursor)->value, hd_expand));

            (*cursor)->value = NULL;
            *cursor = (*cursor)->next;
        }
    }
    return (0);
}
