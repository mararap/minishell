# heredoc redirection
`<<` is the most special redirection, because unlike `<`, it does **not** open an existing file.

Instead, minishell:

1. parses `<< delimiter`
2. **collects lines from the user first**
3. stores them in a temporary file
4. reopens that temp file for reading
5. later redirects `STDIN` from that file

So the command eventually reads heredoc content as if it were ordinary input.

## The full flow

```text
lexer -> parser -> heredoc preparation -> execution
 <<       redir      read until delimiter   dup2(temp_fd, STDIN)
```

---

## 1) Lexer: `<<` becomes `TOKEN_HEREDOC`

In `src/lexer/lexer.c`, `ms_add_redir_token()` checks:

```c
if (lstate->line[lstate->i] == '<' && lstate->line[lstate->i + 1] == '<')
	type = TOKEN_HEREDOC;
```

So for:

```bash
cat << EOF
```

the lexer creates a heredoc token, not a normal input-redirection token.

It also sets:

```c
lstate->expect_heredoc_delim = (type == TOKEN_HEREDOC);
```

That matters because the **next word is the delimiter**, and our lexer treats it specially.

---

## 2) Delimiter handling: the word after `<<` is collected without normal expansion

Still in `src/lexer/lexer.c`, when the next word is lexed:

```c
wctx.allow_expansion = !lstate->expect_heredoc_delim;
```

So right after `<<`, `allow_expansion` becomes `0`.

That means the delimiter token is collected as a literal delimiter word, not as a normal expandable shell word.

Example:

```bash
cat << "$USER"
```

The delimiter token keeps the quoted form information, and later the parser uses that to decide whether heredoc body expansion is allowed.

---

## 3) Parser: `<< delimiter` becomes a heredoc redirection node

In `src/parser/handle_redirs.c`, `ms_process_redir_token()` handles `TOKEN_HEREDOC`.

It requires the next token to be a `TOKEN_WORD`, otherwise it prints syntax error.

Then it decides whether heredoc body expansion should happen:

```c
hd_expand = 0;
if (type == REDIR_HEREDOC && (*cursor)->quoted == 0)
	hd_expand = 1;
```

So:

* `<< EOF` → `heredoc_expand = 1`
* `<< "EOF"` → `heredoc_expand = 0`
* `<< 'EOF'` → `heredoc_expand = 0`

That matches shell behavior: **quoted delimiter disables `$` expansion in the heredoc body**.

Then it creates a `t_redir` node:

```c
ms_create_redir(type, target, hd_expand, ambiguous)
```

That redirection node stores:

* `type = REDIR_HEREDOC`
* `target = delimiter`
* `heredoc_expand`
* `heredoc_fd` initially `-1`

So after parsing:

```bash
cat << EOF
```

the command has a heredoc redirection attached to it.

---

## 4) Heredocs are prepared before the pipeline executes

In `src/shell/loop.c`, before `ms_execute_pipeline()` runs, our shell does:

```c
status = ms_prepare_heredocs(shell, commands);
if (status == 0)
	status = ms_execute_pipeline(shell, commands);
```

This is very important.

It means heredoc input is collected **before** normal execution begins.

So for:

```bash
cat << EOF
hello
EOF
```

our shell first gathers the heredoc content, stores it, and only then starts the command pipeline.

---

## 5) Preparing one heredoc: build temp input and save its fd

In `src/heredoc/heredoc_prepare.c`, `ms_prepare_heredocs()` walks every command and every redirection.

For each heredoc it calls:

```c
fd = ms_build_one_heredoc(shell, cmds, redir, &lines_read);
```

If successful:

```c
redir->heredoc_fd = fd;
```

So the heredoc redirection now owns a readable file descriptor containing the collected text.

That fd will later be plugged into `STDIN`.

---

## 6) How the heredoc content is actually collected

This happens in `src/heredoc/heredoc_build.c`.

### Step A: create a temp file

`ms_build_one_heredoc()` calls:

```c
job.wfd = ms_hd_open_tmp(&job.path);
```

In `src/heredoc/heredoc_tmp.c`, `ms_hd_open_tmp()` creates files like:

```text
/tmp/minishell_hd_0
/tmp/minishell_hd_1
...
```

using:

```c
open(path, O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0600);
```

So heredoc content is stored in a real temporary file, not in memory and not directly in a pipe.

### Step B: fork a child to read heredoc input

Then `ms_build_one_heredoc()` does:

```c
pid = fork();
if (pid == 0)
	ms_hd_run_child(&job);
```

So heredoc reading happens in a separate child process.

The parent temporarily ignores `SIGINT` and `SIGQUIT` while waiting.

---

## 7) The heredoc child reads until the delimiter

In `src/heredoc/heredoc_read.c`, the child loop is:

```c
while (1)
{
	line = ms_hd_read_line(shell);
	if (g_signal_number == SIGINT)
		return (130);
	...
	if (!line)
	{
		ms_hd_warn_eof(redir);
		return (...);
	}
	if (ft_strcmp(line, redir->target) == 0)
	{
		free(line);
		return (...);
	}
	if (ms_hd_write_line(shell, redir, wfd, line) != 0)
		return (1);
}
```

So it keeps reading lines until one of three things happens:

### A) User presses Ctrl+C

Then it aborts with status `130`.

### B) EOF arrives before delimiter

Then it prints a warning:

```text
warning: here-document at line ... delimited by end-of-file (wanted `EOF')
```

### C) Input line exactly matches the delimiter

Then the heredoc ends normally.

---

## 8) What prompt is shown during heredoc

In `ms_hd_read_line()`:

```c
write(STDERR_FILENO, HEREDOC_PROMPT, ft_strlen(HEREDOC_PROMPT));
line = ms_get_next_line(STDIN_FILENO);
```

And `HEREDOC_PROMPT` is:

```c
# define HEREDOC_PROMPT "> "
```

So interactive heredoc input looks like:

```text
> hello
> world
> EOF
```

This is separate from the normal shell prompt.

---

## 9) Expansion inside heredoc body

When a line is read, `ms_hd_write_line()` decides whether to expand variables:

```c
if (!redir->heredoc_expand)
{
	write(wfd, line, ft_strlen(line));
	write(wfd, "\n", 1);
	...
}
expanded = ms_expand_heredoc_line(shell, line);
```

So:

### Unquoted delimiter

```bash
cat << EOF
$USER
EOF
```

`heredoc_expand == 1`, so `$USER` is expanded.

### Quoted delimiter

```bash
cat << "EOF"
$USER
EOF
```

`heredoc_expand == 0`, so `$USER` is written literally.

That behavior is controlled entirely by the delimiter’s `quoted` flag from parsing.

---

## 10) The temp file is reopened for reading

After the child finishes, `ms_hd_finalize_build()` in `src/heredoc/heredoc_build.c` does:

```c
rfd = open(job->path, O_RDONLY | O_CLOEXEC);
unlink(job->path);
```

So the process is:

1. write heredoc content to temp file
2. reopen temp file read-only
3. unlink the pathname
4. keep only the file descriptor

That is a good pattern because the heredoc content is now accessible only through the fd, and the temp file name disappears from the filesystem.

The returned `rfd` becomes:

```c
redir->heredoc_fd = rfd;
```

---

## 11) Execution phase: heredoc fd becomes STDIN

Later, when the actual command runs, `ms_apply_redirections()` in `src/redirections.c` handles `REDIR_HEREDOC` with:

```c
static int	ms_apply_heredoc_redir(t_redir *redir)
{
	if (redir->heredoc_fd < 0)
		return (-1);
	if (dup2(redir->heredoc_fd, STDIN_FILENO) < 0)
		...
	close(redir->heredoc_fd);
	redir->heredoc_fd = -1;
	return (0);
}
```

This is the crucial step.

It means:

> “Take the prepared heredoc file descriptor and make it the command’s standard input.”

So from the command’s point of view, it is just reading from stdin.

That is why:

```bash
cat << EOF
hello
EOF
```

works as if `cat` were reading from a file containing `hello\n`.

---

## 12) Cleanup behavior

Our project also cleans up heredoc fds carefully.

### Unused heredoc fds in child processes

In `src/exec/fork.c`, before applying redirections:

```c
ms_close_heredocs(ctx->cmd_list, cmd);
```

This closes heredoc fds belonging to other commands so children do not keep unnecessary descriptors open.

### Final command cleanup

In `src/parser/free_command.c`, if a heredoc fd is still open when freeing commands:

```c
if (redir->type == REDIR_HEREDOC && redir->heredoc_fd >= 0)
	close(redir->heredoc_fd);
```

So heredoc resources are released even on cleanup paths.

---

## 13) One concrete walkthrough

Input:

```bash
cat << EOF
hello $USER
EOF
```

### Lexing

* `cat` → `TOKEN_WORD`
* `<<` → `TOKEN_HEREDOC`
* `EOF` → delimiter word

### Parsing

Create redirection:

```text
type = REDIR_HEREDOC
target = "EOF"
heredoc_expand = 1
heredoc_fd = -1
```

### Preparation

Before execution:

* create `/tmp/minishell_hd_X`
* fork heredoc child
* child reads lines until `EOF`
* expand `$USER` if allowed
* write lines into temp file
* parent reopens temp file read-only
* store fd in `redir->heredoc_fd`

### Execution

When `cat` starts:

* `dup2(redir->heredoc_fd, STDIN_FILENO)`
* `cat` reads heredoc content from stdin

---

## 14) The key difference from `<`

`< file`:

* opens an already existing file

`<< delimiter`:

* creates a temporary input source by reading lines until the delimiter

So heredoc is really:

> “build a custom temporary stdin first, then redirect stdin from it.”

## Bottom line

Our minishell implements `<<` by:

* lexing it as `TOKEN_HEREDOC`
* parsing the following word as a delimiter and recording whether expansion is allowed
* preparing heredoc content **before execution** in a forked helper process
* writing collected lines into a temporary file
* reopening that file as `heredoc_fd`
* finally redirecting the command’s `STDIN` with:

```c
dup2(redir->heredoc_fd, STDIN_FILENO);
```

So in our project, heredoc is implemented as a **temporary file-backed input redirection** that is populated by reading until the delimiter.

