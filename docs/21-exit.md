Here’s how `exit` is implemented in our minishell.

## Why `exit` is special

`exit` has to terminate the **shell itself**, so it must run in the **parent shell process** when it is a standalone command.

Your project handles that correctly:

* `exit` is recognized as a builtin
* it is marked as a builtin that **needs the parent**
* when there is exactly **one command**, the executor runs it with `ms_run_builtin_parent(...)`

That is what allows `exit` to stop the main shell loop instead of just killing a child.

---

## Where `exit` is dispatched

In `src/builtin/builtins.c`:

```c
if (ft_strncmp(argv[0], "exit", 5) == 0)
	return (ms_builtin_exit(shell, argv));
```

And it is marked as parent-only here:

```c
if (ft_strncmp(cmd_name, "exit", 5) == 0)
	return (1);
```

Then in `src/exec/executor.c`:

```c
if (ms_count_commands(command_list) == 1
	&& ms_use_parent_builtin(command_list))
	return (ms_run_builtin_parent(shell, command_list));
```

So plain:

```bash
exit
```

runs in the real shell process.

---

## The actual builtin

The implementation is in `src/builtin/builtin_exit.c`:

```c
int	ms_builtin_exit(t_shell *shell, char **argv)
```

The key helper is:

```c
static int	ms_mark_exit(t_shell *shell, int status)
{
	shell->should_exit = 1;
	return (status);
}
```

That is the heart of the builtin.

It does **not** call `exit(...)` directly.
Instead, it sets:

```c
shell->should_exit = 1;
```

and returns the status code that should become the shell’s final exit status.

That design is good, because it lets the program unwind normally through the main loop and cleanup code.

---

## How `exit` with no arguments works

This is the main required case:

```c
if (!argv[1])
	return (ms_mark_exit(shell, shell->last_exit_status));
```

So for:

```bash
exit
```

your shell:

1. sets `shell->should_exit = 1`
2. returns `shell->last_exit_status`

So `exit` with no arguments exits the shell using the status of the most recently executed command or pipeline.

That matches normal shell behavior.

---

## Why the shell actually stops

Your main loop in `src/shell/loop.c` is:

```c
while (!shell->should_exit)
```

So once `ms_builtin_exit()` sets:

```c
shell->should_exit = 1;
```

the next loop check fails, and the shell stops reading commands.

Then `main()` in `src/shell/main.c` does:

```c
ms_main_loop(&shell);
ms_free_shell(&shell);
...
return (shell.last_exit_status);
```

So the shell exits cleanly and returns the stored status to the OS.

---

## Where the final exit status is stored

After command execution, `src/shell/loop.c` does:

```c
status = ms_execute_pipeline(shell, commands);
shell->last_exit_status = status;
```

So when `exit` returns a status, that status is copied into:

```c
shell->last_exit_status
```

Then `main()` finally returns that value.

So the complete path is:

```text
exit builtin
-> sets should_exit = 1
-> returns status
-> shell->last_exit_status = status
-> main loop stops
-> main returns shell.last_exit_status
```

---

## Interactive `exit` message

Your implementation prints `exit` when appropriate:

```c
if (shell->is_interactive && getpid() == shell->main_pid)
	write(STDOUT_FILENO, "exit\n", 5);
```

So in the real interactive parent shell, typing `exit` prints:

```text
exit
```

before the shell terminates.

The `getpid() == shell->main_pid` check is important: it avoids printing that message from forked children.

---

## Important nuance: this builtin does more than just “no options”

Although the project requirement says **“exit with no options”**, your implementation actually supports a bit more than that.

### 1. `exit --`

This branch exists:

```c
if (ft_strncmp(argv[1], "--", 3) == 0)
{
	if (argv[2])
		return (ms_exit_too_many());
	return (ms_mark_exit(shell, shell->last_exit_status));
}
```

So `exit --` is accepted as a special case.

### 2. Numeric arguments

Your code also supports:

```bash
exit 42
```

using:

```c
ms_atoll_strict(argv[1], &value)
```

and then:

```c
return (ms_mark_exit(shell, (unsigned char)value));
```

So numeric arguments are converted and reduced to shell-style 0–255 behavior via cast to `unsigned char`.

### 3. Invalid numeric arguments

If the argument is not a valid integer, it prints:

```text
minishell: exit: ARG: numeric argument required
```

and exits with status `2`.

### 4. Too many arguments

If there is more than one non-special argument:

```c
if (argv[2])
	return (ms_exit_too_many());
```

it prints:

```text
minishell: exit: too many arguments
```

and returns `1` **without** setting `should_exit`.

So the shell stays alive in that case.

---

## Strict numeric parsing

The parser for numeric exit arguments is in `src/builtin/builtin_exit_parse.c`:

```c
int	ms_atoll_strict(const char *s, long long *out)
```

It:

* skips leading/trailing spaces
* accepts optional `+` or `-`
* requires the rest to be digits
* checks overflow against `LLONG_MAX` / `LLONG_MIN`

So invalid inputs like:

* `abc`
* `12x`
* overflowed integers

are rejected cleanly.

---

## One very important pipeline detail

Because `exit` only runs in the parent when there is **one command**, this matters:

```bash
exit | cat
```

In a pipeline, `exit` does **not** terminate the shell.
It runs in a child process, because the executor only chooses the parent-builtin path for a single command.

So:

* standalone `exit` → exits the shell
* `exit` inside a pipeline → exits only that child context

That is the correct behavior.

---

## One concrete walkthrough

For:

```bash
exit
```

your shell does:

1. recognize builtin `exit`
2. since it is a single parent-needed builtin, run it in parent
3. `ms_builtin_exit()` prints `exit\n` if interactive
4. no argument, so:

```c
return (ms_mark_exit(shell, shell->last_exit_status));
```

5. `should_exit` becomes `1`
6. returned status becomes `shell->last_exit_status`
7. main loop ends
8. `main()` returns that status

---

## Bottom line

For the required **“exit with no options”** case, your minishell implements it by:

* running `exit` as a **parent builtin**
* setting:

```c
shell->should_exit = 1;
```

* returning:

```c
shell->last_exit_status
```

Then the main loop stops and `main()` exits with that stored status.

The key line is:

```c
return (ms_mark_exit(shell, shell->last_exit_status));
```

A small honesty note: your current implementation goes beyond the strict minimum, because it also supports `exit --`, numeric arguments, `too many arguments`, and `numeric argument required` handling.
