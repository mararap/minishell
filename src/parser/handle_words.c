/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_words.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 16:29:52 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/22 22:04:28 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_word_has_ifs(const char *word)
{
	if (ft_strchr(word, ' ') || ft_strchr(word, '\t') || ft_strchr(word, '\n'))
		return (1);
	return (0);
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
	free (argv);
	return (new_argv);
}

static int ms_is_export_assign_raw(const t_command *cmd, const t_token *tok)
{
	int i;

	if (!cmd->argv || !cmd->argv[0] || ft_strcmp(cmd->argv[0], "export") != 0)
		return (0);
	if (!tok->raw || (!ft_isalpha(tok->raw[0]) && tok->raw[0] != '_'))
		return (0);
	i = 1;
	while (tok->raw[i] && tok->raw[i] != '=')
	{
		if (!ft_isalnum(tok->raw[i]) && tok->raw[i] != '_')
			return (0);
		i++;
	}
	return (tok->raw[i] == '=');
}

static void ms_add_unsplit_word(t_command *cmd, t_token *tok)
{
	ms_unmask_ifs(tok->value);
	cmd->argv = ms_add_word_to_argv(cmd->argv, tok->value);
	tok->value = NULL;
}

static void	ms_handle_split_word(t_command *cmd, t_token *tok)
{
	char	**parts;
	size_t	i;

	parts = ms_split_ifs_fields(tok->value);
	if (!parts[0] && tok->quoted)
		cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));
	i = 0;
	while (parts[i])
	{
		if (parts[i][0] != '\0' || tok->quoted)
			cmd->argv = ms_add_word_to_argv(cmd->argv, parts[i]);
		else
			free(parts[i]);
		parts[i] = NULL;
		i++;
	}
	free(parts);
	free(tok->value);
	tok->value = NULL;
}

void	ms_handle_word(t_command *cmd, t_token **cursor, t_token *tok)
{
	if (!tok->value)
	{
		*cursor = tok->next;
		return ;
	}
	if ((tok->value[0]) == '\0')
	{
		if (tok->quoted)
			cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));
		free(tok->value);
		tok->value = NULL;
	}
	else if (ms_is_export_assign_raw(cmd, tok))
		ms_add_unsplit_word(cmd, tok);
	else if (ms_word_has_ifs(tok->value))
		ms_handle_split_word(cmd, tok);
	else
		ms_add_unsplit_word(cmd, tok);
	*cursor = tok->next;
}
