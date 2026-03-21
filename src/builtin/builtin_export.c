/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:50 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/20 11:34:15 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_builtin_export(t_shell *shell, char **argv)
{
	int	i;
	int	exit_code;

	if (!argv[1])
	{
		ms_print_export_format(shell->env_list);
		return (0);
	}
	i = 1;
	exit_code = 0;
	while (argv[i])
	{
		if (ms_export_one_arg(shell, argv[i]))
			exit_code = 1;
		i++;
	}
	return (exit_code);
}


