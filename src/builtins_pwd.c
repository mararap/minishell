#include "minishell.h"

int	ms_builtin_pwd(t_shell *shell)
{
	char	buffer[4096];
	char	*cwd;

	(void)shell;
	cwd = getcwd(buffer, sizeof(buffer));
	if (!cwd)
	{
		ms_perror("getcwd");
		return (1);
	}
	write(STDOUT_FILENO, cwd, ft_strlen(cwd));
	write(STDOUT_FILENO, "\n", 1);
	return (0);
}
