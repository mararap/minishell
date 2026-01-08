#include "minishell.h"

int	g_signal_number = 0;

static void	ms_detect_interactive(t_shell *shell, char **argv)
{
	(void)argv;
	if (isatty(STDIN_FILENO))
		shell->is_interactive = 1;
	else
		shell->is_interactive = 0;
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	ms_init_shell(&shell, envp);
	ms_detect_interactive(&shell, argv);
	ms_main_loop(&shell);
	ms_free_shell(&shell);
	return (shell.last_exit_status);
}
