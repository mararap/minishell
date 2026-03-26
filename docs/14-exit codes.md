In our minishell, `$?` is handled with a very clear two-step design:

1. the shell keeps the latest status in `shell->last_exit_status`
2. when the lexer sees `$?`, it replaces it with that integer converted to a string

So the important question is really:

* **where is `last_exit_status` updated?**
* **where is `$?` expanded?**

---

## 1) Where `$?` is expanded

The special case is in `src/expand/expand_variable.c`:

```c
if (str[*idx + 1] == '?')
{
	*idx = *idx + 2;
	return (ft_itoa(shell->last_exit_status));
}
```

So when the lexer encounters `$?`:

* it consumes the two characters `$` and `?`
* it reads `shell->last_exit_status`
* it converts that number to text with `ft_itoa()`
* that text becomes part of the current word

### Example

If:

```text
shell->last_exit_status == 127
```

then:

```bash
echo $?
```

is lexed as if the user had written:

```bash
echo 127
```

---

## 2) When that expansion happens

It happens during **word collection in the lexer**, not later in execution.

In `src/lexer/collect_word.c`:

```c
if (wctx->str[*wctx->idx] == '$' && wctx->allow_expansion)
	return (ms_expand_variable(wctx->shell, wctx->str, wctx->idx));
```

So `$?` is expanded while building the token value.

That means by the time parsing starts, the token already contains the numeric string.

---

## 3) Where the shell stores the “most recent status”

Your shell stores it here in `t_shell`:

```c
typedef struct s_shell
{
	t_env_var	*env_list;
	int			last_exit_status;
	...
}	t_shell;
```

So `$?` is just reading this field:

```c
shell->last_exit_status
```

### Initial value

In `src/shell/init.c`:

```c
shell->last_exit_status = 0;
```

So before any command runs, `$?` starts as `0`.

---

## 4) Where `last_exit_status` gets updated after a command or pipeline

The central update happens in `src/shell/loop.c`:

```c
static void	ms_execute_commands(t_shell *shell, t_command *commands)
{
	int	status;

	status = ms_prepare_heredocs(shell, commands);
	if (status == 0)
		status = ms_execute_pipeline(shell, commands);
	shell->last_exit_status = status;
	ms_free_command_list(commands);
}
```

This is the key line:

```c
shell->last_exit_status = status;
```

So after the current command line finishes, the shell remembers that result.
Then the next time the user types `$?`, that stored value is expanded.

---

# The “foreground pipeline” part

Your subject wording says:

> “most recently executed foreground pipeline”

That matches your implementation well, because your shell does **not** implement background jobs with `&`.
So every pipeline it runs is effectively a foreground pipeline.

Examples:

* `ls`
* `echo hi | wc -c`
* `cat < in | grep x > out`

all run in the foreground, and their resulting status ends up in `shell->last_exit_status`.

---

## 5) How pipeline status is computed

The real pipeline status comes from `src/exec/executor_wait.c`.

Your executor waits for all children, but it stores the status of the **last command in the pipeline**.

### Key idea

In `ms_wait_for_children()`:

```c
state.last_pid = last_pid;
...
ms_store_last_status(pid, &state, status);
```

And in `ms_store_last_status()`:

```c
if (pid != state->last_pid)
	return ;
state->last_status = ms_status_to_exit(status);
```

So only the child whose PID matches the **last pipeline command** decides the final pipeline status.

That means for:

```bash
false | true
```

your minishell stores the status of `true`, not `false`.

That is the normal shell rule for pipelines.

---

## 6) How exit codes are converted

Still in `src/exec/executor_wait.c`:

```c
static int	ms_status_to_exit(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
```

So:

### Normal exit

If the last command exits normally:

* `exit 0` → `$? = 0`
* `exit 42` → `$? = 42`

### Killed by signal

If the last command is terminated by a signal:

* `SIGINT` → `$? = 130`
* `SIGQUIT` → `$? = 131`

because the shell uses:

```text
128 + signal_number
```

That matches standard shell behavior.

---

## 7) Single builtins also feed `$?`

For a single builtin that runs in the parent, `ms_execute_pipeline()` does this:

```c
if (ms_count_commands(command_list) == 1
	&& ms_use_parent_builtin(command_list))
	return (ms_run_builtin_parent(shell, command_list));
```

So for commands like:

* `cd`
* `export`
* `unset`
* `exit`

the returned builtin status becomes the `status` seen in `ms_execute_commands()`, and then:

```c
shell->last_exit_status = status;
```

So `$?` also works correctly after parent-run builtins.

Example:

```bash
cd /does/not/exist
echo $?
```

The second line expands from the status returned by `cd`.

---

## 8) Parse/syntax failures also affect `$?`

Even though the subject sentence focuses on executed pipelines, your code also updates `last_exit_status` for shell-side errors.

### Unclosed quotes

In `src/lexer/unclosed_quotes.c`:

```c
shell->last_exit_status = 2;
```

### Parse failure

In `src/shell/loop.c`:

```c
shell->last_exit_status = 2;
```

So after syntax errors, `$?` becomes `2`, which is shell-like behavior.

---

## 9) Heredoc interruption also feeds into `$?`

Since heredocs are prepared before execution, `ms_execute_commands()` first does:

```c
status = ms_prepare_heredocs(shell, commands);
if (status == 0)
	status = ms_execute_pipeline(shell, commands);
shell->last_exit_status = status;
```

So if heredoc preparation is interrupted, that status is stored too.

This means `$?` reflects not only successful command execution, but also pre-execution failures that the shell treats as the result of the current command line.

---

## 10) One full example

Take:

```bash
/bin/false
echo $?
```

### First line

* executor runs `/bin/false`
* `waitpid()` gets its exit status
* `ms_status_to_exit()` returns `1`
* `ms_execute_commands()` sets:

```c
shell->last_exit_status = 1;
```

### Second line

When lexer sees `$?`:

* `ms_expand_variable()` detects the `?`
* returns `ft_itoa(shell->last_exit_status)`
* so `$?` becomes `"1"`

Then `echo` prints:

```text
1
```

---

## 11) Pipeline example

Input:

```bash
/bin/false | /bin/true
echo $?
```

### What happens

* both commands are forked
* shell waits for all children
* only the **last child** (`/bin/true`) decides the final pipeline status
* `ms_status_to_exit()` returns `0`
* `shell->last_exit_status = 0`

So the next line prints:

```text
0
```

That is exactly what “most recently executed foreground pipeline” means in your code.

---

## 12) Signal example

Input:

```bash
cat
```

then press `Ctrl+C`, then run:

```bash
echo $?
```

If the foreground command was terminated by `SIGINT`, your code does:

```c
return (128 + WTERMSIG(status));
```

So:

* `WTERMSIG(status) == SIGINT == 2`
* stored result becomes `130`

Then `$?` expands to:

```text
130
```

---

## Bottom line

Your minishell handles `$?` by storing the latest command-line result in `shell->last_exit_status`, then expanding `$?` during lexing with:

```c
return (ft_itoa(shell->last_exit_status));
```

And the value stored there comes from:

* the exit code of the **last command in the foreground pipeline**
* or `128 + signal` if that last command was killed by a signal
* or shell-side error statuses like syntax/heredoc failures when applicable

So in your project, `$?` is simply the string form of the shell’s current `last_exit_status`.

I can also draw this as a tiny flow:

`pipeline finishes -> waitpid -> last_exit_status updated -> next line lexes $? -> echo prints it`.
