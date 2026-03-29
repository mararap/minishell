# Input redirection
is implemented in three stages:

```text
lexer  -> parser  -> executor
  <        redir      open + dup2 to STDIN
```

## What `<` means

`< file` means:

> “Before running this command, make file descriptor `0` (`STDIN_FILENO`) read from `file` instead of the terminal.”

So for:

```bash
wc -l < input.txt
```

`wc` does not read from the keyboard. It reads from `input.txt`.

---

## 1) Lexer: `<` becomes a redirection token

In `src/lexer/lexer.c`, the lexer recognizes `<` and creates `TOKEN_REDIR_IN`:

```c
else if (lstate->line[lstate->i] == '<')
	type = TOKEN_REDIR_IN;
```

That happens in `ms_add_redir_token()`.

So input like:

```bash
cat < infile
```

is tokenized roughly as:

```text
TOKEN_WORD("cat")
TOKEN_REDIR_IN
TOKEN_WORD("infile")
```

---

## 2) Parser: `< infile` becomes a `t_redir` node on the command

Our redirection struct is in `include/minishell.h`:

```c
typedef struct s_redir
{
	int				type;
	char			*target;
	int				ambiguous;
	int				heredoc_fd;
	int				heredoc_expand;
	struct s_redir	*next;
}	t_redir;
```

So the parser stores redirections separately from `argv`.

### Where this happens

In `src/parser/fill_command.c`:

* normal words go to `argv`
* non-word tokens like `<` go to `ms_process_redir_token()`

```c
if (tok->type == TOKEN_WORD)
	ms_handle_word(cmd, cursor, tok);
else if (ms_process_redir_token(cmd, cursor, tok) < 0)
	return (-1);
```

Then in `src/parser/handle_redirs.c`:

```c
type = ms_token_to_redir_type(tok->type);
...
*cursor = (*cursor)->next;
if (!*cursor || (*cursor)->type != TOKEN_WORD)
	return (ms_print_syntax_error(*cursor), -1);
...
target = ms_redir_target_value(type, *cursor, &ambiguous);
ms_redir_add_back(&cmd->redirections, ms_create_redir(type, target,
		hd_expand, ambiguous));
```

So for:

```bash
cat < infile
```

the parser builds one command roughly like:

```text
argv = ["cat", NULL]
redirections = [ { type = REDIR_IN, target = "infile" } ]
```

### Syntax protection

If `<` is not followed by a word, parser rejects it.

Examples:

* `cat <`
* `cat < | wc`

Those trigger syntax error in `ms_process_redir_token()`.

---

## 3) Executor: `<` is applied before the command runs

The actual input redirection logic is in `src/redirections.c`.

### The key function

For `<`, our shell uses `ms_apply_input_redir()`:

```c
static int	ms_apply_input_redir(t_redir *redir)
{
	int	fd;

	fd = open(redir->target, O_RDONLY);
	if (fd < 0)
	{
		ms_redir_sys_error(redir->target);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		close(fd);
		perror("dup2");
		return (-1);
	}
	close(fd);
	return (0);
}
```

This is the full mechanism:

### Step A: open the file read-only

```c
fd = open(redir->target, O_RDONLY);
```

So `<` always opens the target in read mode.

### Step B: replace standard input

```c
dup2(fd, STDIN_FILENO);
```

This is the most important line.

It means:

* whatever file descriptor `fd` refers to
* becomes file descriptor `0` (`STDIN_FILENO`)

After that, anything reading from standard input will actually read from the file.

### Step C: close the extra descriptor

```c
close(fd);
```

After `dup2`, the original `fd` is no longer needed, because `STDIN_FILENO` now points to the same open file description.

---

## 4) When this function is called

In `src/redirections.c`, `ms_apply_redirections()` loops through all redirections of the command:

```c
if (redirections->type == REDIR_IN
	&& ms_apply_input_redir(redirections) < 0)
	return (-1);
```

So every redirection attached to the command is applied before execution.

---

## 5) For external commands: redirection is applied in the child before `execve`

In `src/exec/fork.c`, inside `ms_execute_child()`:

```c
if (ms_apply_redirections(cmd->redirections) < 0)
	ms_child_exit(ctx, 1);
```

Only after that does the shell continue to the builtin or external command:

```c
status = ms_exec_external_command(ctx->shell, cmd->argv);
```

So the order is:

```text
fork
-> set up pipes
-> apply redirections
-> exec command
```

That is why `cat < infile` works: by the time `cat` starts, its stdin is already the file.

---

## 6) For parent-run builtins: stdin is temporarily replaced, then restored

There is a second important path: builtins that must run in the parent, like `cd`, `export`, `unset`, `exit`.

In `src/builtin/builtins.c`, `ms_run_builtin_parent()` does this:

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

So for parent-executed builtins, the shell:

1. saves the real stdin/stdout/stderr
2. applies `<` redirection
3. runs the builtin
4. restores the original descriptors

That prevents the shell itself from staying permanently redirected after the builtin finishes.

---

## 7) Error handling for `<`

If the input file cannot be opened, `ms_apply_input_redir()` prints an error and fails:

```c
if (fd < 0)
{
	ms_redir_sys_error(redir->target);
	return (-1);
}
```

So:

```bash
cat < does_not_exist
```

prints something like:

```text
does_not_exist: No such file or directory
```

and the command is not executed normally.

---

## 8) Ambiguous redirect handling

Before execution, the parser may mark a redirection target as ambiguous in `src/parser/handle_redirs.c`.

Then `ms_apply_redirections()` checks:

```c
if (redirections->ambiguous)
{
	write(STDERR_FILENO, redirections->target,
		ft_strlen(redirections->target));
	write(STDERR_FILENO, ": ambiguous redirect\n", 21);
	return (-1);
}
```

So if expansion/splitting makes the `<` target invalid, the redirection fails before `open()`.

---

## 9) Multiple input redirections: the last one wins

Because redirections are stored in a linked list and applied in order, this:

```bash
cmd < a < b
```

results in:

1. stdin redirected to `a`
2. then stdin redirected again to `b`

So the final stdin is `b`.

That behavior comes naturally from repeatedly calling:

```c
dup2(fd, STDIN_FILENO);
```

left to right in `ms_apply_redirections()`.

---

## 10) One full example

For:

```bash
wc -l < infile
```

our shell does this:

### Lexing

```text
TOKEN_WORD("wc")
TOKEN_WORD("-l")
TOKEN_REDIR_IN
TOKEN_WORD("infile")
```

### Parsing

Builds:

```text
command.argv = ["wc", "-l", NULL]
command.redirections = [ REDIR_IN -> "infile" ]
```

### Execution

In child:

```text
open("infile", O_RDONLY) -> fd
dup2(fd, STDIN_FILENO)
close(fd)
execve(... "wc" ...)
```

After that, `wc` reads from `infile` through standard input.

---

## Bottom line

Our minishell implements `<` by:

* lexing `<` as `TOKEN_REDIR_IN` in `src/lexer/lexer.c`
* parsing the following word into a `t_redir` node in `src/parser/handle_redirs.c`
* applying it before execution with `open(target, O_RDONLY)` and `dup2(fd, STDIN_FILENO)` in `src/redirections.c`

So “redirect input” in our project literally means:
**replace file descriptor 0 with the opened input file before the command runs.**
