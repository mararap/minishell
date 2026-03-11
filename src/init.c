/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 16:10:01 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/11 10:43:58 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void ms_bootstrap_env(t_shell *shell)
{
	char *cwd;

	//Ensure PWD exists even when started with env -i
	if (!ms_env_get_value(shell->env_list, "PWD"))
	{
		cwd = getcwd(NULL, 0);
		if (cwd)
		{
			ms_env_set(&shell->env_list, "PWD", cwd, 1);
			free(cwd);
		}
	}

	//Optional: when env -i compability
	if (!ms_env_get_value(shell->env_list, "SHLVL"))
		ms_env_set(&shell->env_list, "SHLVL", "1", 1);
	if (!ms_env_get_value(shell->env_list, "PATH"))
		ms_env_set(&shell->env_list, "PATH", "/usr/local/bin:/usr/bin:/bin", 1);
}

void	ms_init_shell(t_shell *shell, char **envp)
{
	char	*shlvl_str;
	int		shlvl;
	char	*new_shlvl;
	
	shell->env_list = ms_env_from_environ(envp);
	ms_bootstrap_env(shell);
	if (isatty(STDIN_FILENO))
	{
		shlvl_str = ms_env_get_value(shell->env_list, "SHLVL");
		if (shlvl_str)
			shlvl = ft_atoi(shlvl_str) + 1; //incremet SHLVL by one on startup
		else
			shlvl = 1;
		new_shlvl = ft_itoa(shlvl);
		ms_env_set(&shell->env_list, "SHLVL", new_shlvl, 1);
		free(new_shlvl);
	}
	shell->last_exit_status = 0;
	shell->is_interactive = 0;
	shell->input_line_num = 1;
	shell->should_exit = 0;
	shell->main_pid = getpid();
	shell->current_line = NULL;
	rl_signal_event_hook = ms_rl_event_hook;
}

void	ms_free_shell(t_shell *shell)
{
	ms_env_free_list(&shell->env_list);
	shell->env_list = NULL;
	rl_clear_history();
}
