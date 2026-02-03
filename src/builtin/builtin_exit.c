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
 *   0–255 range like Bash (e.g., `exit 256` becomes 0). :contentReference[oaicite:2]{index=2}
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


static int ms_is_numeric(char *str)
{
    int i;

    if (!str || !*str)
        return (0);

    i = 0;
    if (str[i] == '+' || str[i] == '-')
    {
        if (!str[i + 1] || !ft_isdigit(str[i + 1]))
            return (0);
        i++;
    }
    while (str[i])
    {
        if (!ft_isdigit(str[i]))
            return (0);
        i++;
    }
    return (1);
}

int ms_builtin_exit(t_shell *shell, char **argv)
{
    int exit_code;

    (void)shell;

    /* No argument: exit with code 0 */
    if (!argv[1])
        exit(0);

    /* First argument must be numeric */
    if (!ms_is_numeric(argv[1]))
    {
        write(STDERR_FILENO, SHELL_NAME ": exit: numeric argument required\n", 41);
        exit(2);
    }

    /* Too many arguments: error, but do not exit shell */
    if (argv[2])
    {
        write(STDERR_FILENO, SHELL_NAME ": exit: too many arguments\n", 28);
        return (1);
    }

    /* Convert first argument using ft_atoi and exit */
    exit_code = ft_atoi(argv[1]);
    exit((unsigned char)exit_code);

    return (0); /* Unreachable, but keeps the compiler happy */
}
