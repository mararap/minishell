#include "minishell.h"

void	ms_init_shell(t_shell *shell, char **envp)
{
	shell->env_list = ms_env_from_environ(envp);
	shell->last_exit_status = 0;
	shell->is_interactive = 0;
}

void	ms_free_shell(t_shell *shell)
{
	ms_env_free_list(shell->env_list);
	shell->env_list = NULL;
}
