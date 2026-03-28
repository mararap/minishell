Here’s how `pwd` is implemented in your `minishell.zip`.

## What `pwd` is supposed to do

The `pwd` builtin prints the shell’s **current working directory** to standard output, followed by a newline.

In your project, `pwd` does that by calling `getcwd()` directly.

That is important because it means your shell prints the **real current directory from the kernel**, not just the value stored in the `PWD` environment variable.

---

## Where `pwd` is recognized as a builtin

In `src/builtin/builtin_is_builtin.c`:

```c
if (ft_strncmp(cmd_name, "pwd", 4) == 0)
	return (1);
```

So the shell knows `pwd` is one of the builtins.

Then in `src/builtin/builtins.c` it is dispatched here:

```c
if (ft_strncmp(argv[0], "pwd", 4) == 0)
	return (ms_builtin_pwd(shell));
```

So when the executor decides a command is a builtin, `pwd` is routed to `ms_builtin_pwd(...)`.

---

## Does `pwd` have to run in the parent?

No.

Unlike `cd`, `export`, `unset`, and `exit`, your `pwd` builtin is **not** marked as a builtin that must run in the parent process.

In `src/builtin/builtins.c`, only these are parent-required:

```c
if (ft_strncmp(cmd_name, "cd", 3) == 0)
	return (1);
if (ft_strncmp(cmd_name, "export", 7) == 0)
	return (1);
if (ft_strncmp(cmd_name, "unset", 6) == 0)
	return (1);
if (ft_strncmp(cmd_name, "exit", 5) == 0)
	return (1);
```

Since `pwd` only prints information and does not need to modify the parent shell state, this is correct.

That means:

* `pwd` can safely run in the child execution path
* `pwd` still works in pipelines
* it does not need special parent-only handling like `cd`

---

## The actual implementation

The real logic is in `src/builtin/builtin_pwd.c`:

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

This function is very direct.

Its flow is:

1. ignore the `shell` argument because `pwd` does not need shell state
2. call `getcwd(NULL, 0)` to allocate and retrieve the current directory
3. if `getcwd()` fails, print an error with `perror("getcwd")` and return `1`
4. otherwise write the directory path to `stdout`
5. write a newline
6. free the allocated buffer
7. return `0`

---

## Why `getcwd(NULL, 0)` matters

This call asks the system to:

* determine the current working directory
* allocate enough memory for the full path

So your implementation does **not** rely on a fixed-size buffer.

That is a good and simple design for this project.

It also means the output comes from the actual current directory of the process, not from `shell->env_list`.

---

## Error handling

If `getcwd()` fails, your code does:

```c
perror("getcwd");
return (1);
```

So the error message format comes from the system, for example:

```text
getcwd: <system error message>
```

and the builtin exits with status `1`.

On success it returns `0`.

---

## Output behavior

On success, your `pwd` prints exactly:

```text
<current-directory>\n
```

because it does:

```c
write(STDOUT_FILENO, cwd, ft_strlen(cwd));
write(STDOUT_FILENO, "\n", 1);
```

So a command like:

```bash
pwd
```

produces something like:

```text
/home/user/minishell
```

with a trailing newline.

---

## Relationship with `PWD`

Your shell also bootstraps the `PWD` variable during initialization if it is missing.

However, your `pwd` builtin does **not** print that variable directly.

Instead, it always asks the operating system for the real current directory using `getcwd()`.

So even if `PWD` were stale or manually modified, `pwd` would still print the actual directory of the running shell process.

---

## Arguments and options

Your implementation does **not** parse options.

That matches the project requirement well: **`pwd` with no options**.

Also, `ms_builtin_pwd()` receives only `shell`, not `argv`, so it does not inspect extra arguments at all.

That means the current implementation simply prints the working directory and returns success, rather than validating unsupported `pwd` arguments.

For the 42 mandatory requirement, this is acceptable because the subject only asks for `pwd` with no options.

---

## One concrete example

For:

```bash
pwd
```

your shell does roughly this:

```text
recognize builtin pwd
-> dispatch to ms_builtin_pwd(shell)
-> call getcwd(NULL, 0)
-> write the returned path to stdout
-> write "\n"
-> free the allocated path
-> return 0
```

If `getcwd()` failed, it would instead do:

```text
perror("getcwd")
-> return 1
```

---

## Bottom line

Your `pwd` implementation is simple and correct for the minishell mandatory part because it:

* recognizes `pwd` as a builtin
* dispatches it to `ms_builtin_pwd(...)`
* calls `getcwd()` to retrieve the real current directory
* prints that directory followed by a newline
* frees allocated memory
* returns `0` on success and `1` on failure

The most important line is this one:

```c
cwd = getcwd(NULL, 0);
```

That is the actual operation that makes your `pwd` print the current working directory.
