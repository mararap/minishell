#include "minishell.h"

int	ms_builtin_pwd(t_shell *shell)
{
	char	*cwd;

	(void)shell;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		ms_perror("getcwd");
		return (1);
	}
	write(STDOUT_FILENO, cwd, ft_strlen(cwd));
	write(STDOUT_FILENO, "\n", 1);
	free(cwd);
	return (0);
}
