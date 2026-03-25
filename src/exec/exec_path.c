/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+
	+:+     */
/*   By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+
	+#+        */
/*                                                +#+#+#+#+#+
	+#+           */
/*   Created: 2026/03/20 21:30:00 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ms_is_executable_file(char *candidate)
{
	struct stat	st;

	if (stat(candidate, &st) != 0 || S_ISDIR(st.st_mode))
		return (0);
	return (access(candidate, X_OK) == 0);
}

static void	ms_remember_denied_path(char **saved, char *candidate)
{
	struct stat	st;

	if (*saved || access(candidate, F_OK) != 0)
		return ;
	if (stat(candidate, &st) != 0 || S_ISDIR(st.st_mode))
		return ;
	if (access(candidate, X_OK) != 0)
		*saved = ft_strdup(candidate);
}

static char	*ms_join_search_dir(char *dir, char *cmd)
{
	if (dir[0] == '\0')
		return (ms_str_join_three(".", "/", cmd));
	return (ms_str_join_three(dir, "/", cmd));
}

static char	*ms_search_path_dirs(char **paths, char *cmd, int *used_path)
{
	char	*candidate;
	char	*denied_path;
	int		i;

	denied_path = NULL;
	i = 0;
	while (paths[i])
	{
		candidate = ms_join_search_dir(paths[i], cmd);
		if (ms_is_executable_file(candidate))
		{
			free(denied_path);
			ms_free_str_array(paths);
			if (used_path)
				*used_path = 1;
			return (candidate);
		}
		ms_remember_denied_path(&denied_path, candidate);
		free(candidate);
		i++;
	}
	ms_free_str_array(paths);
	if (denied_path)
	{
		if (used_path)
			*used_path = 1;
		return (denied_path);
	}
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
