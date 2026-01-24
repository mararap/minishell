#include "minishell.h"

/*
** signals.c
**
** Handles signal configuration for minishell.
**
** Interactive mode:
**  - SIGINT  (Ctrl-C): clears current input and redisplays prompt
**  - SIGQUIT (Ctrl-\): ignored
**
** Child processes:
**  - SIGINT and SIGQUIT restored to default behavior
**
** Notes:
**  - Uses sigaction() for reliable signal handling
**  - SA_RESTART is enabled in interactive mode
**  - g_signal_number tracks last received signal
*/

static void	ms_sigint_interactive(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}

int	ms_rl_event_hook(void)
{
	if (g_signal_number == SIGINT)
	{
		g_signal_number = 0;
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	return (0);
}

void	ms_setup_interactive_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	ft_bzero(&sa_int, sizeof(sa_int));
	ft_bzero(&sa_quit, sizeof(sa_quit));

	sa_int.sa_handler = ms_sigint_interactive;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;

	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;

	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

void	ms_setup_child_signals(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}
