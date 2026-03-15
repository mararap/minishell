#include "minishell.h"

/*
 * minishell: builtin exit implementation
 *
 * The `exit` command is a *special builtin* that terminates the current
 * shell process and returns a status code to the parent process if one
 * is provided. This implementation replicates the behavior of Bash’s
 * `exit` builtin as closely as makes sense for a minishell project.
 *
 * Synopsis:
 *   exit [n]
 *
 * Behavior:
 * - If no argument is given, the shell exits with status 0.
 * - If a single numeric argument `n` is provided, the shell exits with
 *   that value cast to an unsigned 8-bit value (0–255).
 * - Leading white spaces will be skipped.
 * - If the first argument is not a valid integer, prints an error to
 *   stderr and exits with status 2. This covers cases like "foo" or "+"
 *   which are not numeric. :contentReference[oaicite:1]{index=1}
 * - If more than one argument is given *after* a valid numeric first
 *   argument, prints a “too many arguments” error to stderr and *does not*
 *   exit the shell; instead, returns a failure status (typically 1).
 *
 * Numeric argument rules:
 * - A valid numeric argument may have an optional leading '+' or '-' and
 *   must contain at least one digit. Strings like "+" or "-" alone are
 *   not considered valid. Only digits after the optional sign are allowed.
 *
 * Edge cases and notes:
 * - Casting to `unsigned char` ensures the exit status wraps into the
 *   0–255 range like Bash (e.g.,
		`exit 256` becomes 0). :contentReference[oaicite:2]{index=2}
 * - Bash builtins typically return 2 for incorrect usage — but since
 *   `exit` actually terminates the shell in many error cases, this shell

	*   prints the message and exits with code 2 directly. :contentReference[oaicite:3]{index=3}
 *
 * Example behavior:
 *   exit           → terminates shell with status 0
 *   exit 42        → terminates shell with status 42
 *   exit -5        → terminates with status 251 (two’s complement)
 *   exit foo       → prints error, exits status 2
 *   exit 1 2       → prints “too many arguments”, returns 1 (shell stays)
 */

static void	ms_exit_prefix(void)
{
	write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
	write(STDERR_FILENO, ": exit: ", 8);
}

static int	ms_exit_too_many(void)
{
	ms_exit_prefix();
	write(STDERR_FILENO, "too many arguments\n", 19);
	return (1);
}

static int	ms_exit_numeric_error(char *arg)
{
	ms_exit_prefix();
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, ": numeric argument required\n", 28);
	return (2);
}

static int	ms_mark_exit(t_shell *shell, int status)
{
	shell->should_exit = 1;
	return (status);
}

static const char	*ms_skip_spaces(const char *s)
{
	while (*s && ft_isspace((unsigned char)*s))
		s++;
	return (s);
}

static int	ms_parse_ull(const char **s, unsigned long long *value,
		unsigned long long limit)
{
	int	digit;

	*value = 0;
	if (!ft_isdigit((unsigned char)**s))
		return (0);
	while (ft_isdigit((unsigned char)**s))
	{
		digit = **s - '0';
		if (*value > limit / 10 || (*value == limit / 10
				&& (unsigned long long)digit > limit % 10))
			return (0);
		*value = (*value * 10) + digit;
		(*s)++;
	}
	return (1);
}

static int	ms_finish_atoll_parse(const char *s, int sign,
		unsigned long long value, long long *out)
{
	s = ms_skip_spaces(s);
	if (*s != '\0')
		return (0);
	if (sign < 0 && value == (unsigned long long)LLONG_MAX + 1)
		*out = LLONG_MIN;
	else
		*out = (long long)value * sign;
	return (1);
}

static int	ms_atoll_strict(const char *s, long long *out)
{
	unsigned long long	value;
	unsigned long long	limit;
	int					sign;

	s = ms_skip_spaces(s);
	sign = 1;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	limit = (unsigned long long)LLONG_MAX + (sign < 0);
	if (!ms_parse_ull(&s, &value, limit))
		return (0);
	return (ms_finish_atoll_parse(s, sign, value, out));
}

int	ms_builtin_exit(t_shell *shell, char **argv)
{
	long long	value;

	if (shell->is_interactive && getpid() == shell->main_pid)
		write(STDOUT_FILENO, "exit\n", 5);
	if (!argv[1])
		return (ms_mark_exit(shell, shell->last_exit_status));
	if (ft_strncmp(argv[1], "--", 3) == 0)
	{
		if (argv[2])
			return (ms_exit_too_many());
		return (ms_mark_exit(shell, shell->last_exit_status));
	}
	if (argv[1][0] == '\0' || !ms_atoll_strict(argv[1], &value))
		return (ms_mark_exit(shell, ms_exit_numeric_error(argv[1])));
	if (argv[2])
		return (ms_exit_too_many());
	return (ms_mark_exit(shell, (unsigned char)value));
}
