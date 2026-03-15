/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 15:25:19 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/15 21:07:21 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * minishell: builtin echo implementation (supports only -n flag)
 *
 * This function implements a simplified version of the shell builtin
 * `echo` command. The primary purpose of `echo` is to print its
 * arguments to standard output, separated by spaces, and optionally
 * followed by a newline.
 *
 * The -n flag
 * ===========
 * In many common shells, `echo -n` suppresses the trailing newline that
 * normally follows the output. For example:
 *
 *   $ echo hello world
 *   hello world
 *
 *   $ echo -n hello world
 *   hello world$
 *
 * (the prompt appears immediately because no newline was printed).
 *
 * This implementation supports only this `-n` behavior: if one or more
 * consecutive arguments are valid `-n` flags, the trailing newline is
 * omitted. All other arguments are printed normally.
 *
 * Valid -n flags
 * ===============
 * To provide behavior similar to Bash and other shells, this code
 * recognizes any argument that begins with `-` followed by one or more
 * `n` characters (e.g., "-n", "-nn", "-nnnn") as a valid flag that
 * suppresses the newline. Any argument that contains characters other
 * than `n` after the leading hyphen is NOT treated as a flag and will
 * be printed as a normal argument.
 *
 * The helper function `ms_is_valid_n_flag` performs this check by
 * scanning each character after the initial '-' and returning true
 * only if all such characters are 'n'.
 *
 * This logic matches common interactive shells that treat repeated `-n`
 * flags as valid until the first non-flag argument is encountered.
 *
 * Argument printing
 * =================
 * After skipping any `-n` flags at the beginning of the argv list, all
 * remaining arguments are printed with a single space between them.
 * No extra space is printed after the final argument. If no arguments
 * remain and the newline is not suppressed, only a newline is printed.
 *
 * Behavior differences and standards
 * =================================
 * - This implementation is tailored for a minishell project and is not
 *   a full POSIX echo implementation; only `-n` is supported.
 *
 * - According to the POSIX standard, the echo utility *shall not accept
 *   any options*; whether a leading "-n" is treated as a flag or as

	*   literal text is *implementation-defined* for different shells/utilities. :contentReference[oaicite:0]{index=0}
 *
 * - Common shells like Bash and Zsh do treat `-n` as a flag, so this
 *   implementation mimics that widely used behavior rather than strict
 *   POSIX semantics. :contentReference[oaicite:1]{index=1}
 *
 * Example behavior
 * ================
 *   echo foo bar           -> prints "foo bar\n"
 *   echo -n foo bar        -> prints "foo bar" (no newline)
 *   echo -nnn foo bar      -> prints "foo bar" (many -n flags)
 *   echo -- -n foo         -> prints "-- -n foo\n"  (first non-flag)
 *
 * Return value
 * ============
 * The function always returns 0. In a full shell, builtin return
 * statuses often reflect success/failure of the command. For this simple
 * echo, success is assumed.
 */

static int	ms_is_valid_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] == '\0')
		return (0);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

static int	ms_echo_skip_flags(char **argv, int *print_newline)
{
	int	i;

	i = 1;
	*print_newline = 1;
	while (argv[i] && ms_is_valid_n_flag(argv[i]))
	{
		*print_newline = 0;
		i++;
	}
	return (i);
}

static int	ms_echo_write(char *s, size_t len)
{
	if (write(STDOUT_FILENO, s, len) < 0)
		return (1);
	return (0);
}

static int	ms_echo_print_args(char **argv, int start)
{
	int	first;

	first = 1;
	while (argv[start])
	{
		if (!first && ms_echo_write(" ", 1))
			return (1);
		if (ms_echo_write(argv[start], ft_strlen(argv[start])))
			return (1);
		first = 0;
		start++;
	}
	return (0);
}

static int	ms_ignore_sigpipe(struct sigaction *old_action)
{
	struct sigaction	new_action;

	ft_bzero(&new_action, sizeof(new_action));
	new_action.sa_handler = SIG_IGN;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	if (sigaction(SIGPIPE, &new_action, old_action) < 0)
		return (1);
	return (0);
}

static void ms_restore_sigpipe(struct sigaction *old_action)
{
	sigaction(SIGPIPE, old_action, NULL);
}

int	ms_builtin_echo(char **argv)
{
	struct sigaction	old_action;
	int					start;
	int					print_newline;
	int					status;

	start = ms_echo_skip_flags(argv, &print_newline);
	if (ms_ignore_sigpipe(&old_action))
		return (1);
	status = ms_echo_print_args(argv, start);
	if (!status && print_newline)
		status = ms_echo_write("\n", 1);
	ms_restore_sigpipe(&old_action);
	return (status);
}
