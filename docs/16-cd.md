Here’s how `pwd` is implemented in your `minishell.zip`.

## What `pwd` should do

`pwd` prints the shell’s current working directory followed by a newline.

In your project, that behavior is implemented very directly: it asks the OS for the current directory with `getcwd()` and writes it to stdout.

---

## 1) `pwd` is recognized as a builtin

In `src/builtin/builtin_is_builtin.c`:

```c
if (ft_strncmp(cmd_name, "pwd", 4) == 0)
	return (1);
```

So when the executor sees `argv[0] == "pwd"`, it knows this is a builtin and does not search `PATH` or call `execve()`.

---

## 2) `pwd` is dispatched through the builtin table

In `src/builtin/builtins.c`:

```c
if (ft_strncmp(argv[0], "pwd", 4) == 0)
	return (ms_builtin_pwd(shell));
```

So once the command is identified as `pwd`, your shell calls:

```c
int	ms_builtin_pwd(t_shell *shell)
```

from `src/builtin/builtin_pwd.c`.

---

## 3) `pwd` does not need to run in the parent

Your project treats only these builtins as “must run in parent”:

* `cd`
* `export`
* `unset`
* `exit`

That list is in `ms_builtin_needs_parent()`.

`pwd` is **not** there, so it can safely run in a child process, because printing the current directory does not need to modify the shell’s own state.

So unlike `cd`, `pwd` does not need special parent execution.

---

## 4) The actual implementation is very small

In `src/builtin/builtin_pwd.c`:

```c
int	ms_builtin_pwd(t_shell *shell)
{
	char	*cwd;

	(void)shell;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("getcwd");
		return (1);
	}
	write(STDOUT_FILENO, cwd, ft_strlen(cwd));
	write(STDOUT_FILENO, "\n", 1);
	free(cwd);
	return (0);
}
```

This is the whole builtin.

### Step by step

#### `getcwd(NULL, 0)`

```c
cwd = getcwd(NULL, 0);
```

This asks the OS for the absolute current working directory.

Using `NULL, 0` means `getcwd()` allocates enough memory itself.

So if the shell is currently in:

```text
/home/jatanaso/projects/minishell
```

then `cwd` becomes that string.

---

#### Error handling

```c
if (!cwd)
{
	perror("getcwd");
	return (1);
}
```

If `getcwd()` fails, your shell prints a system error like:

```text
getcwd: <system error message>
```

and returns status `1`.

---

#### Print the directory

```c
write(STDOUT_FILENO, cwd, ft_strlen(cwd));
write(STDOUT_FILENO, "\n", 1);
```

So `pwd` prints:

* the current directory
* then a newline

That matches the expected builtin behavior.

---

#### Free the allocated buffer

```c
free(cwd);
```

Because `getcwd(NULL, 0)` allocated memory.

---

#### Return success

```c
return (0);
```

So on success, `pwd` sets exit status `0`.

---

## 5) “with no options” in your implementation

Your current `ms_builtin_pwd()` takes only `t_shell *shell` and does **not** inspect `argv` at all.

So in practice, it behaves like:

* `pwd` → prints cwd
* `pwd -x` → still prints cwd
* `pwd something` → still prints cwd

In other words, your code does **not** parse or reject options/arguments.
It simply prints the current working directory unconditionally.

So the builtin matches the required core behavior of `pwd`, but it is looser than a strict “no options allowed” validation.

That is an important implementation detail.

---

## 6) Redirections still work with `pwd`

Even though `pwd` itself only does:

```c
write(STDOUT_FILENO, cwd, ft_strlen(cwd));
```

redirections still work because the shell applies redirections before builtin execution.

So this works naturally:

```bash
pwd > out.txt
```

because by the time `ms_builtin_pwd()` writes to `STDOUT_FILENO`, stdout may already be redirected to a file.

---

## 7) One concrete flow

For:

```bash
pwd
```

your shell does:

```text
recognize builtin "pwd"
-> dispatch to ms_builtin_pwd()
-> getcwd(NULL, 0)
-> write cwd to STDOUT
-> write "\n"
-> free buffer
-> return 0
```

For:

```bash
pwd > out.txt
```

the flow is:

```text
set up redirection first
-> run ms_builtin_pwd()
-> write goes to out.txt instead of terminal
```

---

## Bottom line

Your `pwd` builtin is implemented in the simplest possible way:

* recognize `pwd` as a builtin
* call `ms_builtin_pwd()`
* get the current working directory with `getcwd(NULL, 0)`
* print it to stdout with a newline
* return `0` on success, `1` on failure

The core line is:

```c
cwd = getcwd(NULL, 0);
```

followed by:

```c
write(STDOUT_FILENO, cwd, ft_strlen(cwd));
```

Next is `export`.
