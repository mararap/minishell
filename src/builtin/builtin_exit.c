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
    while (ft_isspace(str[i]))
        i++;
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

static int ms_atoll_strict(const char *s, long long *out)
{
    long long result;
    int sign;

    result = 0;
    sign = 1;

    /* skip leading whitespace */
    while (*s == ' ' || (*s >= 9 && *s <= 13))
        s++;

    if (*s == '+' || *s == '-')
    {
        if (*s == '-')
            sign = -1;
        s++;
    }

    if (!ft_isdigit(*s))
        return (0);

    while (ft_isdigit(*s))
    {
        if (result > (9223372036854775807LL - (*s - '0')) / 10)
            return (0); /* overflow */
        result = result * 10 + (*s - '0');
        s++;
    }

    /* skip trailing whitespace */
    while (*s == ' ' || (*s >= 9 && *s <= 13))
        s++;

    if (*s != '\0')
        return (0);

    *out = result * sign;
    return (1);
}

int ms_builtin_exit(t_shell *shell, char **argv)
{
    long long value;

    (void)shell;

    /* Bash prints "exit" only in interactive shells */
    if (isatty(STDIN_FILENO))
        write(STDOUT_FILENO, "exit\n", 5);

    /* No argument -> exit 0*/
    if (!argv[1])
        exit(0);

    /* Handle `exit --` */
    if (ft_strncmp(argv[1], "--", 3) == 0)
    {
        if (argv[2])
        {
            write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
            write(STDERR_FILENO, ": exit: ", 8);
            write(STDERR_FILENO, "too many arguments\n", 19);
            return (1);
        }
        exit(0);
    }

    /* First argument must be numeric AND within range */
    if (!ms_is_numeric(argv[1]) || !ms_atoll_strict(argv[1], &value)
        || argv[1][0] == '\0')
    {
        write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
        write(STDERR_FILENO, ": exit: ", 8);
        write(STDERR_FILENO, argv[1], ft_strlen(argv[1]));
        write(STDERR_FILENO, ": numeric argument required\n", 28);
        exit(2);
    }

    /* Too many arguments → error, do NOT exit */
    if (argv[2])
    {
        write(STDERR_FILENO, SHELL_NAME, ft_strlen(SHELL_NAME));
        write(STDERR_FILENO, ": exit: ", 8);
        write(STDERR_FILENO, "too many arguments\n", 19);
        return (1);
    }

    /* Valid single numeric argument */
    exit((unsigned char)value);
}
