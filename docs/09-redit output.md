# Output redirection
is implemented in the usual 3 stages:

```text
lexer -> parser -> executor
  >      redir      open file + dup2 to STDOUT
```

## What `>` means

For a command like:

```bash
echo hello > out.txt
```

the shell should:

1. open `out.txt` for writing
2. truncate it if it already exists
3. make that file become `STDOUT_FILENO` (`fd 1`)
4. run the command

So `echo` writes into the file instead of the terminal.

---

## 1) Lexer: `>` becomes a redirection token

In `src/lexer/lexer.c`, `>` is recognized in `ms_add_redir_token()`:

```c
else if (lstate->line[lstate->i] == '<')
	type = TOKEN_REDIR_IN;
else
	type = TOKEN_REDIR_OUT;
```

So:

```bash
echo hello > out.txt
```

is tokenized roughly as:

```text
TOKEN_WORD("echo")
TOKEN_WORD("hello")
TOKEN_REDIR_OUT
TOKEN_WORD("out.txt")
```

If the lexer sees `>>`, it creates `TOKEN_REDIR_APPEND` instead, but plain `>` becomes `TOKEN_REDIR_OUT`.

---

## 2) Parser: `>` + target becomes a redirection node

In `src/parser/fill_command.c`, non-word tokens are handled by `ms_process_redir_token()`:

```c
if (tok->type == TOKEN_WORD)
	ms_handle_word(cmd, cursor, tok);
else if (ms_process_redir_token(cmd, cursor, tok) < 0)
	return (-1);
```

Then in `src/parser/handle_redirs.c`, the parser:

* converts `TOKEN_REDIR_OUT` to `REDIR_OUT`
* checks that the next token exists and is a `TOKEN_WORD`
* extracts the target filename
* stores it in the command’s redirection list

Key part:

```c
type = ms_token_to_redir_type(tok->type);
...
*cursor = (*cursor)->next;
if (!*cursor || (*cursor)->type != TOKEN_WORD)
	return (ms_print_syntax_error(*cursor), -1);
...
ms_redir_add_back(&cmd->redirections, ms_create_redir(type, target,
	hd_expand, ambiguous));
```

So after parsing:

```bash
echo hello > out.txt
```

we get one command roughly like:

```text
argv = ["echo", "hello", NULL]
redirections = [ { type = REDIR_OUT, target = "out.txt" } ]
```

### Syntax protection

If `>` is missing its target, parser rejects it.

Examples:

* `echo hi >`
* `echo hi > | cat`

Those produce a syntax error before execution.

---

## 3) Executor: `>` opens the file and redirects stdout

The real work happens in `src/redirections.c`, in `ms_apply_output_redir()`:

```c
static int	ms_apply_output_redir(t_redir *redir)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (redir->type == REDIR_OUT)
		flags |= O_TRUNC;
	else
		flags |= O_APPEND;
	fd = open(redir->target, flags, 0666);
	if (fd < 0)
	{
		ms_redir_sys_error(redir->target);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		close(fd);
		perror("dup2");
		return (-1);
	}
	close(fd);
	return (0);
}
```

For plain `>` the important flags are:

```c
O_WRONLY | O_CREAT | O_TRUNC
```

That means:

* open for writing
* create the file if it does not exist
* truncate it to empty if it already exists

### The key line

This is the actual redirection:

```c
dup2(fd, STDOUT_FILENO);
```

That replaces file descriptor `1` with the opened file.

After that, anything written to standard output goes into the file.

Then the original `fd` is closed:

```c
close(fd);
```

because it is no longer needed once `STDOUT_FILENO` points to the same open file.

---

## 4) Where `>` is applied

`ms_apply_redirections()` loops through the command’s redirection list:

```c
if ((redirections->type == REDIR_OUT
		|| redirections->type == REDIR_APPEND)
	&& ms_apply_output_redir(redirections) < 0)
	return (-1);
```

So every `>` or `>>` attached to the command is applied before the command runs.

---

## 5) External commands: redirection happens in the child before `execve`

In `src/exec/fork.c`, inside `ms_execute_child()`:

```c
if (ms_apply_redirections(cmd->redirections) < 0)
	ms_child_exit(ctx, 1);
```

Only after that does it execute the command.

So the order is:

```text
fork
-> set up pipe fds
-> apply redirections
-> execve(...)
```

That is why:

```bash
/bin/echo hello > out.txt
```

writes into `out.txt` and not to the terminal.

---

## 6) Builtins run in parent: stdout is saved and restored

For builtins that must run in the parent (`cd`, `export`, `unset`, `exit`), our shell still supports `>` correctly.

In `src/builtin/builtins.c`, `ms_run_builtin_parent()` does:

```c
if (ms_dup_stdio(&save_in, &save_out, &save_err) < 0)
	return (perror("dup"), 1);
...
if (cmd->redirections && ms_apply_redirections(cmd->redirections) < 0)
	status = 1;
else
	status = ms_run_builtin_child(shell, cmd->argv);
ms_restore_stdio(save_in, save_out, save_err);
```

So for a parent-run builtin with `>`:

1. save current stdin/stdout/stderr
2. apply `>` redirection
3. run builtin
4. restore original stdio

That prevents the shell prompt itself from staying redirected afterward.

---

## 7) Error handling

If the file cannot be opened, `ms_apply_output_redir()` prints a system-style error:

```c
if (fd < 0)
{
	ms_redir_sys_error(redir->target);
	return (-1);
}
```

So something like:

```bash
echo hi > /no/such/dir/out
```

prints an error and the command does not run normally.

---

## 8) Ambiguous redirects are rejected first

Before open/dup2 even happens, `ms_apply_redirections()` checks:

```c
if (redirections->ambiguous)
{
	write(STDERR_FILENO, redirections->target,
		ft_strlen(redirections->target));
	write(STDERR_FILENO, ": ambiguous redirect\n", 21);
	return (-1);
}
```

So if expansion produced an invalid output target, the shell reports `ambiguous redirect` instead of trying `open()`.

---

## 9) Multiple `>` redirections: the last one wins

Because redirections are stored in order and applied in order, this:

```bash
echo hi > a > b
```

does:

1. redirect stdout to `a`
2. redirect stdout again to `b`

So the final output goes to `b`.

That happens naturally because `dup2(fd, STDOUT_FILENO)` is called again for each redirection in sequence.

---

## 10) Nice detail with pipes: `>` overrides pipe output for that command

In `src/exec/fork.c`, the child first connects the pipe:

```c
ms_dup_child_fds(ctx, cmd)
```

and only then applies redirections:

```c
ms_apply_redirections(cmd->redirections)
```

So if a command has both a pipe and `>`:

```bash
echo hi > out | cat
```

the later `>` redirection overrides the earlier pipe stdout setup for that command.

So the command writes to `out`, not into the pipe.

---

## One full walkthrough

For:

```bash
echo hello > out.txt
```

### Lexing

```text
TOKEN_WORD("echo")
TOKEN_WORD("hello")
TOKEN_REDIR_OUT
TOKEN_WORD("out.txt")
```

### Parsing

```text
argv = ["echo", "hello", NULL]
redirections = [ REDIR_OUT -> "out.txt" ]
```

### Execution

```text
open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666)
dup2(fd, STDOUT_FILENO)
close(fd)
run echo
```

So `echo` writes to the file through standard output.

## Bottom line

Our minishell implements `>` by:

* lexing `>` as `TOKEN_REDIR_OUT`
* parsing the following word into a redirection node on the command
* applying it before execution with:

```c
open(target, O_WRONLY | O_CREAT | O_TRUNC, 0666);
dup2(fd, STDOUT_FILENO);
```

So “redirect output” in our code literally means:
**replace file descriptor 1 with the opened target file before running the command.**
