/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 16:30:48 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/16 13:23:15 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_redir_raw_has_quotes(const char *raw)
{
	while (raw && *raw)
	{
		if (*raw == '\'' || *raw == '"')
			return (1);
		raw++;
	}
	return (0);
}

static char	*ms_redir_ambiguous_target(t_token *target)
{
	if (target->raw)
		return (ft_strdup(target->raw));
	return (ft_strdup(""));
}

static char	*ms_redir_split_target(t_token *target, int *ambiguous)
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

static char	*ms_redir_target_value(int type, t_token *target, int *ambiguous)
{
	char	*resolved;

	if (type == REDIR_HEREDOC)
	{
		ms_unmask_ifs(target->value);
		*ambiguous = 0;
		return (target->value);
	}
	if (target->value[0] == '\0' && !target->quoted
		&& !ms_redir_raw_has_quotes(target->raw))
	{
		*ambiguous = 1;
		return (ms_redir_ambiguous_target(target));
	}
	if (target->value[0] == '\0')
	{
		*ambiguous = 0;
		return (ft_strdup(""));
	}
	if (!ft_strchr(target->value, ' ') && !ft_strchr(target->value, '\t')
		&& !ft_strchr(target->value, '\n'))
	{
		resolved = ft_strdup(target->value);
		ms_unmask_ifs(resolved);
		*ambiguous = 0;
		return (resolved);
	}
	return (ms_redir_split_target(target, ambiguous));
}

int	ms_process_redir_token(t_command *cmd, t_token **cursor, t_token *tok)
{
	int		type;
	int		hd_expand;
	int		ambiguous;
	char	*target;

	type = ms_token_to_redir_type(tok->type);
	if (type < 0)
	{
		ms_print_syntax_error(tok);
		return (-1);
	}
	*cursor = (*cursor)->next;
	if (!*cursor || (*cursor)->type != TOKEN_WORD)
	{
		ms_print_syntax_error(*cursor);
		return (-1);
	}
	hd_expand = 0;
	if (type == REDIR_HEREDOC && (*cursor)->quoted == 0)
		hd_expand = 1;
	target = ms_redir_target_value(type, *cursor, &ambiguous);
	ms_redir_add_back(&cmd->redirections, ms_create_redir(type, target,
			hd_expand, ambiguous));
	if (type == REDIR_HEREDOC)
		(*cursor)->value = NULL;
	*cursor = (*cursor)->next;
	return (0);
}
