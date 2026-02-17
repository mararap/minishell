/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 16:10:01 by marapovi          #+#    #+#             */
/*   Updated: 2026/02/17 09:32:20 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ms_init_shell(t_shell *shell, char **envp)
{
	char	*shlvl_str;
	int		shlvl;
	char	*new_shlvl;
	
	shell->env_list = ms_env_from_environ(envp);
	shlvl_str = ms_env_get_value(shell->env_list, "SHLVL");
	if (shlvl_str)
		shlvl = ft_atoi(shlvl_str) + 1; //incremet SHLVL by one on startup
	else
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	ms_env_set(&shell->env_list, "SHLVL", new_shlvl, 1);
	free(new_shlvl);
	shell->last_exit_status = 0;
	shell->is_interactive = 0;
	rl_signal_event_hook = ms_rl_event_hook;
}

void	ms_free_shell(t_shell *shell)
{
	ms_env_free_list(&shell->env_list);
	shell->env_list = NULL;
}
