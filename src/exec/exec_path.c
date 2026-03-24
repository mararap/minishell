/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*ms_join_search_dir(char *dir, char *cmd)
{
	if (dir[0] == '\0')
		return (ms_str_join_three(".", "/", cmd));
	return (ms_str_join_three(dir, "/", cmd));
}

static char	*ms_search_path_dirs(char **paths, char *cmd, int *used_path)
{
	char		*candidate;
	struct stat	st;
	int			i;

	i = 0;
	while (paths[i])
	{
		candidate = ms_join_search_dir(paths[i], cmd);
		if (access(candidate, F_OK) == 0 && stat(candidate, &st) == 0
			&& !S_ISDIR(st.st_mode))
		{
			ms_free_str_array(paths);
			if (used_path)
				*used_path = 1;
			return (candidate);
		}
		free(candidate);
		i++;
	}
	ms_free_str_array(paths);
	return (NULL);
}

char	*ms_find_executable(t_shell *shell, char *cmd, int *used_path)
{
	char	*path_env;
	char	**paths;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (used_path)
		*used_path = 0;
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = ms_env_get_value(shell->env_list, "PATH");
	if (!path_env || path_env[0] == '\0')
		return (ft_strdup(cmd));
	paths = ms_split_path_keep_empty(path_env);
	if (!paths)
		return (NULL);
	return (ms_search_path_dirs(paths, cmd, used_path));
}

void	ms_update_underscore(t_shell *shell, char *value)
{
	if (!shell || !value || value[0] == '\0')
		return ;
	ms_env_set(&shell->env_list, "_", value, 1);
}

void	ms_adjust_envp_shlvl(char **envp)
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
