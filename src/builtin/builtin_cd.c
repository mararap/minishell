/*
## Description

This file implements the `cd` builtin command for the minishell project.
It handles changing the current working directory, validating arguments, reporting errors, and updating the `PWD` and `OLDPWD` environment variables according to shell behavior.

---

### `cd_error`

```c
static int cd_error(const char *msg);
```

This helper function prints a formatted `cd` error message to standard error and returns `1`.

* Prefixes the message with `<shell_name>: cd:`
* Used for argument-related errors (missing or too many arguments)
* Centralizes error message formatting for consistency

---

### `ms_update_pwd_vars`

```c
static int ms_update_pwd_vars(t_shell *shell, char *old_pwd);
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
int ms_builtin_cd(t_shell *shell, char **argv);
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
* Reporting meaningful error messages
* Safely managing memory
* Keeping environment variables in sync with the current directory
* Following expected shell behavior

It cleanly separates concerns between error handling, environment updates, and command execution, making the code easy to maintain and extend.
*/


#include "minishell.h"

static int	cd_error(const char *msg)
{
	write(STDERR_FILENO, SHELL_NAME ": cd: ", 7);
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

int	ms_builtin_cd(t_shell *shell, char **argv)
{
	char	*old_pwd;

	old_pwd = getcwd(NULL, 0);
	if (!old_pwd)
	{
		ms_perror("cd: getcwd");
		return 1; // Return after the error is printed
	}

	if (!argv[1])
	{
		return cd_error("missing argument"); // Print error and return 1
	}
	else if (argv[2])
	{
		return cd_error("too many arguments"); // Print error and return 1
	}
	else if (chdir(argv[1]) != 0)
	{
		ms_perror("cd"); // Print error message for chdir failure
		free(old_pwd);
		return 1; // Return after error
	}
	else
	{
		// Update PWD and OLDPWD only after chdir succeeds
		if (ms_update_pwd_vars(shell, old_pwd) != 0)
		{
			free(old_pwd);
			return 1; // Return 1 if updating pwd vars failed
		}
	}

	free(old_pwd);
	return 0; // Success
}