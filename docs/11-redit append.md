`>>` is implemented almost the same way as `>`, with one crucial difference:

* `>` opens the file with **truncate**
* `>>` opens the file with **append**

So `>>` means:

> “Send stdout to this file, but keep the existing contents and write at the end.”

## Full path in your project

```text
lexer -> parser -> executor
 >>      redir      open file in append mode + dup2 to STDOUT
```

---

## 1) Lexer: `>>` becomes `TOKEN_REDIR_APPEND`

In `src/lexer/lexer.c`, `ms_add_redir_token()` checks for the two-character operator first:

```c
if (lstate->line[lstate->i] == '<' && lstate->line[lstate->i + 1] == '<')
	type = TOKEN_HEREDOC;
else if (lstate->line[lstate->i] == '>'
	&& lstate->line[lstate->i + 1] == '>')
	type = TOKEN_REDIR_APPEND;
else if (lstate->line[lstate->i] == '<')
	type = TOKEN_REDIR_IN;
else
	type = TOKEN_REDIR_OUT;
```

So for:

```bash
echo hello >> out.txt
```

the lexer produces roughly:

```text
TOKEN_WORD("echo")
TOKEN_WORD("hello")
TOKEN_REDIR_APPEND
TOKEN_WORD("out.txt")
```

It also advances by 2 characters for `>>`, not 1.

---

## 2) Parser: `>> out.txt` becomes a redirection node

In `src/parser/handle_redirs.c`, `ms_process_redir_token()` takes the redirection token, checks that the next token is a `TOKEN_WORD`, and stores it in the command’s redirection list.

So after parsing:

```bash
echo hello >> out.txt
```

you get one command roughly like:

```text
argv = ["echo", "hello", NULL]
redirections = [ { type = REDIR_APPEND, target = "out.txt" } ]
```

The parser treats it the same general way as other redirections:

* require a target after `>>`
* reject syntax like `echo hi >>`
* keep the target in `cmd->redirections`

---

## 3) Executor: same function as `>`, but with `O_APPEND`

The real difference appears in `src/redirections.c`, inside `ms_apply_output_redir()`:

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
	...
	if (dup2(fd, STDOUT_FILENO) < 0)
	...
	close(fd);
	return (0);
}
```

This is the key logic:

### For `>`

```c
flags = O_WRONLY | O_CREAT | O_TRUNC;
```

### For `>>`

```c
flags = O_WRONLY | O_CREAT | O_APPEND;
```

So with `>>`:

* create the file if it does not exist
* open it for writing
* write at the **end** of the file
* do **not** erase existing content

---

## 4) `dup2()` is what makes stdout go to the file

Still in `ms_apply_output_redir()`:

```c
dup2(fd, STDOUT_FILENO);
```

That replaces file descriptor `1` with the opened file.

After that, the command writes to the file instead of the terminal.

Then the extra descriptor is closed:

```c
close(fd);
```

---

## 5) Where this runs

`ms_apply_redirections()` in `src/redirections.c` handles both `>` and `>>` through the same branch:

```c
if ((redirections->type == REDIR_OUT
		|| redirections->type == REDIR_APPEND)
	&& ms_apply_output_redir(redirections) < 0)
	return (-1);
```

So both operators share the same implementation function; only the open flags differ.

---

## 6) External commands: `>>` is applied before `execve()`

In `src/exec/fork.c`, the child does:

```c
if (ms_apply_redirections(cmd->redirections) < 0)
	ms_child_exit(ctx, 1);
```

before executing the command.

So for:

```bash
/bin/echo hello >> out.txt
```

the child process:

1. opens `out.txt` with append flags
2. redirects stdout to it
3. runs `/bin/echo`

That is why the text is appended to the file.

---

## 7) Parent builtins also support `>>`

For builtins that must run in the parent, `src/builtin/builtins.c` saves stdio, applies redirections, runs the builtin, then restores stdio:

```c
if (cmd->redirections && ms_apply_redirections(cmd->redirections) < 0)
	status = 1;
else
	status = ms_run_builtin_child(shell, cmd->argv);
ms_restore_stdio(save_in, save_out, save_err);
```

So this also works for parent-side builtins:

```bash
export TEST=1 >> out.txt
```

or more realistically:

```bash
echo hi >> out.txt
```

without permanently redirecting the shell prompt.

---

## 8) The semantic difference between `>` and `>>`

### `>`

```bash
echo hello > out.txt
```

If `out.txt` already contains data, it is erased first.

### `>>`

```bash
echo hello >> out.txt
```

If `out.txt` already contains data, the new output is added after the old data.

That difference is entirely controlled by:

* `O_TRUNC` for `>`
* `O_APPEND` for `>>`

---

## 9) Multiple append redirections: the last one still wins

Because redirections are applied in order, something like:

```bash
echo hi >> a >> b
```

will:

1. redirect stdout to `a` in append mode
2. then redirect stdout again to `b` in append mode

So the final output goes to `b`.

“Append” affects how the chosen file is opened, not whether multiple redirections are combined.

---

## 10) One concrete walkthrough

Input:

```bash
echo hello >> out.txt
```

### Lexing

```text
TOKEN_WORD("echo")
TOKEN_WORD("hello")
TOKEN_REDIR_APPEND
TOKEN_WORD("out.txt")
```

### Parsing

```text
argv = ["echo", "hello", NULL]
redirections = [ REDIR_APPEND -> "out.txt" ]
```

### Execution

```text
open("out.txt", O_WRONLY | O_CREAT | O_APPEND, 0666)
dup2(fd, STDOUT_FILENO)
close(fd)
run echo
```

Result:

* `hello` is written at the end of `out.txt`

---

## Bottom line

Your minishell implements `>>` by:

* lexing it as `TOKEN_REDIR_APPEND`
* parsing the next word as its target file
* applying it with `open(target, O_WRONLY | O_CREAT | O_APPEND, 0666)`
* redirecting stdout with `dup2(fd, STDOUT_FILENO)`

So compared to `>`, the only real implementation difference is:

```c
REDIR_OUT    -> O_TRUNC
REDIR_APPEND -> O_APPEND
```

I can explain `<<` next in the same style, since that one is the most different redirection in your project.
