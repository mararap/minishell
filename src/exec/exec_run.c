/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_run.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ms_adjust_envp_shlvl(char **envp)
{
	int		i;
	int		val;
	char	*new_val;
	char	*new_entry;

	i = 0;
	while (envp && envp[i] && ft_strncmp(envp[i], "SHLVL=", 6) != 0)
		i++;
	if (!envp || !envp[i])
		return ;
	val = ft_atoi(envp[i] + 6) - 1;
	if (val < 0)
		return ;
	new_val = ft_itoa(val);
	if (!new_val)
		return ;
	new_entry = ms_str_join_three("SHLVL=", new_val, "");
	free(new_val);
	if (!new_entry)
		return ;
	free(envp[i]);
	envp[i] = new_entry;
}

static int	ms_exec_run_path(t_shell *shell, char **argv, char *path,
		char *display_arg)
{
	char	**envp;
	int		err_no;
	int		status;

	envp = ms_env_to_array(shell->env_list);
	ms_adjust_envp_shlvl(envp);
	execve(path, argv, envp);
	err_no = errno;
	ms_free_str_array(envp);
	status = ms_exec_error_code(display_arg, err_no);
	free(path);
	return (status);
}

int	ms_exec_external_command(t_shell *shell, char **argv)
{
	char	*path;
	char	*display_arg;
	int		used_path;
	int		status;

	path = ms_find_executable(shell, argv[0], &used_path);
	if (!path)
	{
		ms_print_command_not_found(argv[0]);
		return (127);
	}
	display_arg = argv[0];
	if (used_path)
		display_arg = path;
	if (shell && path && path[0])
		ms_env_set(&shell->env_list, "_", path, 1);
	status = ms_exec_precheck(argv[0], display_arg, path);
	if (status >= 0)
	{
		free(path);
		return (status);
	}
	return (ms_exec_run_path(shell, argv, path, display_arg));
}

int	ms_fork_and_execute(t_exec_ctx *ctx, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		ms_setup_child_signals();
		if (cmd->next)
		{
			close(ctx->pipe_fd[0]);
			ctx->pipe_fd[0] = -1;
		}
		if (ctx->pids_to_free)
			free(ctx->pids_to_free);
		ms_execute_child(ctx, cmd);
	}
	return (pid);
}
