Yes. In your `minishell.zip`, pipes are implemented as a full lexer → parser → executor chain.

## What `|` means

For:

```bash
ls | wc -l
```

the shell must make:

* `ls` write to a pipe instead of the terminal
* `wc -l` read from that pipe instead of the keyboard

So the stdout of the left command becomes the stdin of the right command.

---

## 1) Lexer: `|` becomes a pipe token

In `src/lexer/lexer.c`, the lexer turns `|` into `TOKEN_PIPE`:

```c
if (lstate->line[lstate->i] == '|')
{
	ms_tok_add_back(tokens, ms_tok_new(TOKEN_PIPE, NULL, NULL, 0));
	lstate->i++;
	lstate->expect_heredoc_delim = 0;
}
```

So:

```bash
echo hello | wc
```

becomes roughly:

```text
TOKEN_WORD("echo")
TOKEN_WORD("hello")
TOKEN_PIPE
TOKEN_WORD("wc")
```

At this stage, `|` is just syntax information.

---

## 2) Parser: pipe tokens split the input into separate commands

In `src/parser/fill_command.c`, one command is filled until a pipe is reached:

```c
while (*cursor && (*cursor)->type != TOKEN_PIPE)
```

So a `t_command` represents one pipeline segment.

Then in `src/parser/parser.c`, after one command is built, if the cursor is at `TOKEN_PIPE`, parser moves past it and starts a new command:

```c
if (*cursor && (*cursor)->type == TOKEN_PIPE)
{
	*cursor = (*cursor)->next;
	...
}
```

That means:

```bash
echo hello | wc -c | cat
```

becomes a linked list of 3 commands:

### command 1

```text
argv = ["echo", "hello"]
```

### command 2

```text
argv = ["wc", "-c"]
```

### command 3

```text
argv = ["cat"]
```

linked with `cmd->next`.

### Syntax protection

Your parser also rejects invalid pipe placement.

Examples:

* `| ls`
* `echo hi |`
* `echo hi || wc`  ← since `||` is not a supported operator, it becomes a bad pipe sequence here

That happens in `ms_parse_tokens()` / `ms_parse_one_command()` with `ms_print_syntax_error()`.

---

## 3) Executor: pipelines are run command by command

The main entry point is `ms_execute_pipeline()` in `src/exec/executor.c`.

It prepares a `t_pipeline` struct with:

* current command pointer
* `prev_read`
* `pipe_fd[2]`
* array of child PIDs

Important fields from `include/minishell.h`:

```c
typedef struct s_pipeline
{
	t_shell		*shell;
	t_command	*command_list;
	t_command	*cmd;
	pid_t		*pids;
	int			prev_read;
	int			pipe_fd[2];
	int			created;
}	t_pipeline;
```

### `prev_read` is the key

This variable carries:

> “the read end of the previous pipe, which should become stdin for the next command”

At the start:

```c
pl.prev_read = STDIN_FILENO;
```

So the first command normally reads from standard input unless a redirection overrides it.

---

## 4) A new Unix pipe is created for every command except the last one

In `src/exec/executor_spawn.c`, before launching each command:

```c
if (!pl->cmd->next)
	return (0);
if (pipe(pl->pipe_fd) < 0)
```

So:

* if there is a next command, create a real Unix `pipe()`
* if this is the last command, no new pipe is needed

That means in:

```bash
A | B | C
```

the shell creates:

* one pipe between `A` and `B`
* one pipe between `B` and `C`

but none after `C`.

---

## 5) Each command is forked

Still in `src/exec/executor_spawn.c`, each pipeline element is launched through:

```c
pid = ms_fork_and_execute(&ctx, pl->cmd);
```

And `ms_fork_and_execute()` in `src/exec/exec_run.c` does:

```c
pid = fork();
...
if (pid == 0)
{
	ms_setup_child_signals();
	...
	ms_execute_child(ctx, cmd);
}
```

So every pipeline command runs in its own child process.

That is why even builtins inside a pipeline run in a child, not in the shell parent.

Example:

* single `cd` may run in parent
* `cd | cat` must run in pipeline mode, so it runs as a child and cannot affect the parent shell directory

---

## 6) Child-side fd wiring: this is where the pipe actually works

The most important code is in `src/exec/fork.c`, inside `ms_dup_child_fds()`:

```c
if (ms_dup_and_close(ctx->prev_read, STDIN_FILENO) < 0)
...
if (!cmd->next)
	return (0);
if (ms_dup_and_close(ctx->pipe_fd[1], STDOUT_FILENO) < 0)
...
```

This does the real connection.

### For stdin

`ctx->prev_read` becomes `STDIN_FILENO`.

So if this command is not the first one, it reads from the previous pipe.

### For stdout

If there is a next command, `ctx->pipe_fd[1]` becomes `STDOUT_FILENO`.

So this command writes into the new pipe.

### Meaning by position

#### First command in a pipeline

For `A | B | C`:

* stdin = normal stdin
* stdout = write end of pipe 1

#### Middle command

For `B`:

* stdin = read end of pipe 1
* stdout = write end of pipe 2

#### Last command

For `C`:

* stdin = read end of pipe 2
* stdout = normal stdout

That is exactly standard pipeline behavior.

---

## 7) Parent-side fd updates: prepare the next command

After a child is launched, the parent updates its bookkeeping in `ms_update_parent_fds()`:

```c
if (pl->prev_read >= 0 && pl->prev_read != STDIN_FILENO)
	close(pl->prev_read);
...
close(pl->pipe_fd[1]);
pl->prev_read = pl->pipe_fd[0];
```

This is crucial.

After launching one command, the parent:

* closes the old previous read end
* closes the write end of the current pipe
* keeps only the read end of the current pipe
* stores it into `prev_read`

So the next command can use that read end as its stdin.

This is how the pipeline is chained step by step.

---

## 8) Concrete walkthrough: `echo hello | wc -c`

### Step 1: first command = `echo hello`

A pipe is created.

In child 1:

* stdin stays normal
* stdout is duped to `pipe_fd[1]`

So `echo` writes into the pipe.

In parent:

* close write end
* store read end as `prev_read`

### Step 2: second command = `wc -c`

No new pipe is created because this is the last command.

In child 2:

* `prev_read` is duped to stdin
* stdout stays normal terminal output

So `wc -c` reads what `echo` wrote.

That is the whole pipeline.

---

## 9) Three-command example: `A | B | C`

This is the easiest mental model:

### command A

* stdin = terminal
* stdout = pipe1 write end

### command B

* stdin = pipe1 read end
* stdout = pipe2 write end

### command C

* stdin = pipe2 read end
* stdout = terminal

So the data flow is:

```text
A stdout -> pipe1 -> B stdin
B stdout -> pipe2 -> C stdin
```

---

## 10) Redirections are applied after pipe wiring

In `ms_execute_child()`:

```c
if (ms_dup_child_fds(ctx, cmd) < 0)
...
if (ms_apply_redirections(cmd->redirections) < 0)
...
```

This order matters.

It means:

1. pipeline fds are connected first
2. command redirections are applied second

So a redirection can override the pipe.

Example:

```bash
echo hi > out | cat
```

For the first command:

* stdout is first connected to the pipe
* then `>` redirects stdout to `out`

So `echo` writes to `out`, not into the pipe.

That is correct shell behavior.

---

## 11) Waiting and exit status

After all children are created, `ms_finish_pipeline()` calls:

```c
ms_wait_for_children(pl->pids[pl->created - 1], pl->created, ...)
```

And in `src/exec/executor_wait.c`, the shell waits for all children, but stores the exit status of the **last command in the pipeline**.

That matches shell behavior:

```bash
false | true
echo $?
```

should report the status of `true`, not `false`.

Your code does that by remembering `last_pid`.

---

## 12) Signals during pipeline execution

Before launching the pipeline, the parent shell ignores `SIGINT` and `SIGQUIT`:

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

Then children get default signal handling.

That way:

* Ctrl+C affects the running pipeline commands
* the shell itself does not die while waiting

After the pipeline finishes, interactive signal handlers are restored.

---

## 13) Why the fd closing matters

A pipeline only works cleanly if unused ends are closed.

Your code does this carefully:

### In the child

In `ms_fork_and_execute()`:

* if there is a next command, child closes `pipe_fd[0]` because it only needs the write end

In `ms_dup_and_close()`:

* after `dup2(from, to)`, the old fd is closed

### In the parent

In `ms_update_parent_fds()`:

* close old `prev_read`
* close current `pipe_fd[1]`
* keep only the read end needed for the next stage

Without these closes, commands could hang because some process would still keep a pipe end open.

---

## Bottom line

Your minishell implements `|` by:

1. lexing `|` as `TOKEN_PIPE`
2. parsing the token list into a linked list of `t_command` nodes split at each pipe
3. walking that command list in the executor
4. creating a real `pipe()` between adjacent commands
5. using `dup2()` in each child so:

   * previous pipe read end becomes stdin
   * new pipe write end becomes stdout
6. closing unused fds in parent and child
7. waiting for all children and keeping the last command’s exit status

So in your project, a pipeline is literally a chain of children connected by Unix pipe file descriptors.

I can also draw this as a small fd diagram for `cmd1 | cmd2 | cmd3`, showing exactly what `stdin` and `stdout` become in each child.
