/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_words_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 20:59:37 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/25 21:37:53 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_word_has_ifs(const char *word)
{
	if (ft_strchr(word, ' ') || ft_strchr(word, '\t') || ft_strchr(word, '\n'))
		return (1);
	return (0);
}

char	**ms_add_word_to_argv(char **argv, char *word)
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

int	ms_is_export_assign_raw(const t_command *cmd, const t_token *tok)
{
	int	i;

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
