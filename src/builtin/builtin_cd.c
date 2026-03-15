/*
## Description

This file implements the `cd` builtin command for the minishell project.
It handles changing the current working directory, validating arguments,
	reporting errors,
	and updating the `PWD` and `OLDPWD` environment variables according to shell behavior.

---

### `cd_error`

```c
static int	cd_error(const char *msg);
```

This helper function prints a formatted `cd` error message to standard error and returns `1`.

* Prefixes the message with `<shell_name>: cd:`
* Used for argument-related errors (missing or too many arguments)
* Centralizes error message formatting for consistency

---

### `ms_update_pwd_vars`

```c
static int	ms_update_pwd_vars(t_shell *shell, char *old_pwd);
```

This function updates the `PWD` and `OLDPWD` environment variables after a successful directory change.

* Retrieves the new current working directory using `getcwd`
* Sets `OLDPWD` to the previous directory path
* Sets `PWD` to the new current directory
* Frees allocated memory before returning
* Returns `0` on success, `1` on failure

---

### `ms_builtin_cd`

```c
int			ms_builtin_cd(t_shell *shell, char **argv);
```

This is the main implementation of the `cd` builtin command.

#### Behavior:

* Retrieves the current directory before changing it (used for `OLDPWD`)
* Validates arguments:

  * No argument → prints “missing argument” error
  * More than one argument → prints “too many arguments” error
* Attempts to change the directory using `chdir`
* On failure, prints a system error using `ms_perror`
* On success, updates `PWD` and `OLDPWD` using `ms_update_pwd_vars`
* Ensures all allocated memory is properly freed

#### Return values:

* Returns `0` on success
* Returns `1` on any error

---

## Summary

This file provides a robust and modular implementation of the `cd` builtin by:

* Handling argument validation
* Reporting meaningful eimprovmentsrror messages
* Safely managing memory
* Keeping environment variables in sync with the current directory
* Following expected shell behavior

It cleanly separates concerns between error handling, environment updates,
	and command execution, making the code easy to maintain and extend.
*/

#include "minishell.h"

static int	cd_error_with_path(const char *path, const char *msg)
{
	write(STDERR_FILENO, "cd: ", 4);
	if (path)
	{
		write(STDERR_FILENO, path, ft_strlen(path));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
	return (1);
}

static int	ms_update_pwd_vars(t_shell *shell, char *old_pwd)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (1);
	if (old_pwd)
		ms_env_set(&shell->env_list, "OLDPWD", old_pwd, 1);
	ms_env_set(&shell->env_list, "PWD", cwd, 1);
	free(cwd);
	return (0);
}

static void	ms_cd_output(char *str)
{
	write(STDERR_FILENO, "cd: ", 4);
	write(STDERR_FILENO, str, ft_strlen(str));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
	write(STDERR_FILENO, "\n", 1);
}

static int	ms_cd_home(t_shell *shell, char *old_pwd)
{
	char	*home;

	home = ms_env_get_value(shell->env_list, "HOME");
	if (!home || home[0] == '\0')
		return (cd_error_with_path(NULL, "HOME not set"));
	if (chdir(home) != 0)
	{
		ms_cd_output(home);
		return (1);
	}
	return (ms_update_pwd_vars(shell, old_pwd));
}

static int	ms_cd_oldpwd(t_shell *shell, char *old_pwd)
{
	char	*oldpwd;

	oldpwd = ms_env_get_value(shell->env_list, "OLDPWD");
	if (!oldpwd || oldpwd[0] == '\0')
		return (cd_error_with_path(NULL, "OLDPWD not set"));
	if (chdir(oldpwd) != 0)
	{
		ms_cd_output(oldpwd);
		return (1);
	}
	write(STDOUT_FILENO, oldpwd, ft_strlen(oldpwd));
	write(STDOUT_FILENO, "\n", 1);
	return (ms_update_pwd_vars(shell, old_pwd));
}

static char	*ms_cd_get_old_pwd(void)
{
	char	*old_pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
		perror("cd: getcwd");
	return (old_pwd);
}

static int	ms_cd_special_target(t_shell *shell, char **argv, char *old_pwd)
{
	if (!argv[1] || argv[1][0] == '\0')
		return (ms_cd_home(shell, old_pwd));
	if (ft_strncmp(argv[1], "-", 2) == 0 && !argv[2])
		return (ms_cd_oldpwd(shell, old_pwd));
	return (-1);
}

static int	ms_cd_regular_target(t_shell *shell, char **argv, char *old_pwd)
{
	if (argv[2])
		return (cd_error_with_path(NULL, "too many arguments"));
	if (chdir(argv[1]) != 0)
	{
		ms_cd_output(argv[1]);
		return (1); // Return after error
	}
	return (ms_update_pwd_vars(shell, old_pwd));
}
int	ms_builtin_cd(t_shell *shell, char **argv)
{
	char *old_pwd;
	int result;

	old_pwd = ms_cd_get_old_pwd();
	if (!old_pwd)
		return (1);
	result = ms_cd_special_target(shell, argv, old_pwd);
	if (result < 0)
		result = ms_cd_regular_target(shell, argv, old_pwd);
	free(old_pwd);
	return (result);
}