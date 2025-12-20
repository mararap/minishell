#include "minishell.h"

static int	ms_update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	buffer[4096];
	char	*cwd;

	cwd = getcwd(buffer, sizeof(buffer));
	if (!cwd)
		return (1);
	if (old_pwd)
		ms_env_set(&shell->env_list, "OLDPWD", old_pwd);
	ms_env_set(&shell->env_list, "PWD", cwd);
	return (0);
}

int	ms_builtin_cd(t_shell *shell, char **argv)
{
	char	buffer[4096];
	char	*old_pwd;
	int		ret;

	old_pwd = getcwd(buffer, sizeof(buffer));
	if (!argv[1])
	{
		write(STDERR_FILENO, SHELL_NAME ": cd: missing argument\n", 33);
		return (1);
	}
	if (argv[2])
	{
		write(STDERR_FILENO, SHELL_NAME ": cd: too many arguments\n", 33);
		return (1);
	}
	ret = chdir(argv[1]);
	if (ret != 0)
	{
		ms_perror("cd");
		return (1);
	}
	ms_update_pwd_vars(shell, old_pwd);
	return (0);
}
