/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:24:16 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/18 15:11:45 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	cd_error_with_path(const char *path, const char *msg)
{
	write(STDERR_FILENO, "cd: ", 4);
	if (path)
	{
		write(STDERR_FILENO, path, ft_strlen(path));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
	return (1);
}

static int	ms_update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (1);
	if (old_pwd)
		ms_env_set(&shell->env_list, "OLDPWD", old_pwd, 1);
	ms_env_set(&shell->env_list, "PWD", cwd, 1);
	free(cwd);
	return (0);
}

static void	ms_cd_output(char *str)
{
	write(STDERR_FILENO, SHELL_NAME ": cd: ", ft_strlen(SHELL_NAME) + 6);
	write(STDERR_FILENO, str, ft_strlen(str));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	write(STDERR_FILENO, "\n", 1);
}

static int	ms_cd_home(t_shell *shell, char *old_pwd)
{
	char	*home;

	home = ms_env_get_value(shell->env_list, "HOME");
	if (!home || home[0] == '\0')
		return (cd_error_with_path(NULL, "HOME not set"));
	if (chdir(home) != 0)
	{
		ms_cd_output(home);
		return (1);
	}
	return (ms_update_pwd_vars(shell, old_pwd));
}

static int	ms_cd_oldpwd(t_shell *shell, char *old_pwd)
{
	char	*oldpwd;

	oldpwd = ms_env_get_value(shell->env_list, "OLDPWD");
	if (!oldpwd || oldpwd[0] == '\0')
		return (cd_error_with_path(NULL, "OLDPWD not set"));
	if (chdir(oldpwd) != 0)
	{
		ms_cd_output(oldpwd);
		return (1);
	}
	write(STDOUT_FILENO, oldpwd, ft_strlen(oldpwd));
	write(STDOUT_FILENO, "\n", 1);
	return (ms_update_pwd_vars(shell, old_pwd));
}

int	ms_builtin_cd(t_shell *shell, char **argv)
{
	char	*old_pwd;
	int		result;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		return (perror("cd: getcwd"), 1);
	if (!argv[1] || argv[1][0] == '\0')
		result = ms_cd_home(shell, old_pwd);
	else if (ft_strncmp(argv[1], "-", 2) == 0 && !argv[2])
		result = ms_cd_oldpwd(shell, old_pwd);
	else if (argv[2])
		result = cd_error_with_path(NULL, "too many arguments");
	else if (chdir(argv[1]) != 0)
		result = (ms_cd_output(argv[1]), 1);
	else
		result = ms_update_pwd_vars(shell, old_pwd);
	free(old_pwd);
	return (result);
}
