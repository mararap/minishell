/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:30 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/15 15:24:33 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_env_entry(char *name, char *value)
{
	size_t	name_len;
	size_t	val_len;
	size_t	total_len;
	char	*line;

	name_len = ft_strlen(name);
	val_len = ft_strlen(value);
	total_len = name_len + 1 + val_len + 1;
	line = ms_xmalloc(total_len);
	ft_memcpy(line, name, name_len);
	line[name_len] = '=';
	ft_memcpy(line + name_len + 1, value, val_len);
	line[total_len - 1] = '\n';
	write(STDOUT_FILENO, line, total_len);
	free(line);
}

int	ms_builtin_env(t_shell *shell, char **argv)
{
	t_env_var	*iter;

	if (argv[1])
	{
		ft_putstr_fd("env: ", STDERR_FILENO);
		ft_putstr_fd(argv[1], STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	iter = shell->env_list;
	while (iter)
	{
		if (iter->value && iter->exported == 1)
			print_env_entry(iter->name, iter->value);
		iter = iter->next;
	}
	return (EXIT_SUCCESS);
}
