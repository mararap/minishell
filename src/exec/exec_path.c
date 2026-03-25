/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+	+#+           */
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

static char	*ms_return_search_result(char *found_path, int *used_path)
{
	if (found_path)
	{
		if (used_path)
			*used_path = 1;
		return (found_path);
	}
	return (NULL);
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
		if (paths[i][0] == '\0')
			candidate = ms_str_join_three(".", "/", cmd);
		else
			candidate = ms_str_join_three(paths[i], "/", cmd);
		if (ms_is_executable_file(candidate))
		{
			free(denied_path);
			ms_free_str_array(paths);
			return (ms_return_search_result(candidate, used_path));
		}
		ms_remember_denied_path(&denied_path, candidate);
		free(candidate);
		i++;
	}
	ms_free_str_array(paths);
	return (ms_return_search_result(denied_path, used_path));
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
