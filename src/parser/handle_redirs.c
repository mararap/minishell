/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 16:30:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 19:46:22 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_redir_type_or_error(t_token *tok, int *type)
{
	*type = ms_token_to_redir_type(tok->type);
	if (*type < 0)
	{
		ms_print_syntax_error(tok);
		return (-1);
	}
	return (0);
}

static int ms_redir_raw_has_quotes(const char *raw)
{
	while (raw && *raw)
	{
		if (*raw == '\'' || *raw == '"')
			return (1);
		raw++;
	}
	return (0);
}

static int ms_redir_has_ifs(const char *word)
{
	if (ft_strchr(word, ' ') || ft_strchr(word, '\t') || ft_strchr(word, '\n'))
		return (1);
	return (0);
}

/*static int ms_redir_is_ambiguous(t_token *target)
{
	char	**parts;
	size_t	count;

	parts = ms_split_ifs_fields(target->value);
	count = ms_str_arr_len(parts);
	ms_free_str_array(parts);
	if (count)
		return (!ms_redir_raw_has_quotes(target->raw));
	return (count != 1);
}*/

static char *ms_redir_ambiguous_target(t_token *target)
{
	if (target->raw)
		return (ft_strdup(target->raw));
	return (ft_strdup(""));
}

static char *ms_redir_split_target(t_token *target, int *ambiguous)
{
	char	**parts;
	char	*resolved;

	parts = ms_split_ifs_fields(target->value);
	if (ms_str_arr_len(parts) != 1)
	{
		*ambiguous = 1;
		ms_free_str_array(parts);
		return (ms_redir_ambiguous_target(target));
	}
	resolved = ft_strdup(parts[0]);
	ms_free_str_array(parts);
	*ambiguous = 0;
	return (resolved);
}

static char *ms_redir_target_value(int type, t_token *target, int *ambiguous)
{
	char *resolved;

	if (type == REDIR_HEREDOC)
	{
		ms_unmask_ifs(target->value);
		*ambiguous = 0;
		return (target->value);
	}
	if (target->value[0] == '\0')
	{
		if (target->quoted || ms_redir_raw_has_quotes(target->raw))
		{
			*ambiguous = 0;
			return (ft_strdup(""));
		}
		*ambiguous = 1;
		return (ms_redir_ambiguous_target(target));
	}
	if (!ms_redir_has_ifs(target->value))
	{
		resolved = ft_strdup(target->value);
		ms_unmask_ifs(resolved);
		*ambiguous = 0;
		return (resolved);
	}
	return (ms_redir_split_target(target, ambiguous));
}

static int	ms_redir_target_or_error(t_token **cursor)
{
	*cursor = (*cursor)->next;
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
	return (0);
}

static int	ms_redir_expand_flag(int type, t_token *target)
{
	if (type == REDIR_HEREDOC)
		return (target->quoted == 0);
	return (0);
}

int	ms_process_redir_token(t_command *cmd, t_token **cursor, t_token *tok)
{
	int	type;
	int	hd_expand;
	int ambiguous;
	char *target;

	if (ms_redir_type_or_error(tok, &type) < 0)
		return (-1);
	if (ms_redir_target_or_error(cursor) < 0)
		return (-1);
	hd_expand = ms_redir_expand_flag(type, *cursor);
	target = ms_redir_target_value(type, *cursor, &ambiguous);
	ms_redir_add_back(&cmd->redirections, ms_create_redir(type, target, hd_expand, ambiguous));
	if (type == REDIR_HEREDOC)
		(*cursor)->value = NULL;
	*cursor = (*cursor)->next;
	return (0);
}
