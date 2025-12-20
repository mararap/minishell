#include "minishell.h"

static void	ms_sigint_interactive(int signo)
{
	if (signo == SIGINT)
	{
		g_signal_number = signo;
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	ms_setup_interactive_signals(void)
{
	signal(SIGINT, ms_sigint_interactive);
	signal(SIGQUIT, SIG_IGN);
}

void	ms_setup_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
