## 1) The normal shell prompt
is achieved through the **Readline** library, and it is only used when the shell detects it is running interactively.

### 1.1) Where the prompt string is defined

In `include/minishell.h:32`:

```c
# define PROMPT_STR "juma[n]she$ "
```

So the visible prompt text is:

```text
juma[n]she$
```

### 1.2) Where interactive mode is decided

In `src/shell/main.c:17-23`:

```c
static void	ms_detect_interactive(t_shell *shell)
{
	if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
		shell->is_interactive = 1;
	else
		shell->is_interactive = 0;
}
```

This means the shell shows a prompt only when it is connected to a terminal.
When input is piped in or redirected from a file, `is_interactive` becomes `0`, so no prompt is shown.

### 1.3) Where the prompt is actually displayed

In `src/shell/loop.c:63-77`:

```c
static char	*ms_read_line(t_shell *shell)
{
	char	*line;

	if (shell->is_interactive)
	{
		line = readline(PROMPT_STR);
		if (!line)
			write(STDOUT_FILENO, "exit\n", 5);
		return (line);
	}
	line = ms_get_next_line(STDIN_FILENO);
	if (line)
		ms_chomp_eol(line);
	return (line);
}
```

This is the key part:

```c
line = readline(PROMPT_STR);
```

`readline()` does two things at once:

1. **prints the prompt**
2. **waits for the user to type a command**

So the prompt is not printed manually with `write()` for normal commands.
Instead, Readline handles it.

### 1.4) How it fits into the shell loop

In `src/shell/loop.c:80-99`:

```c
void	ms_main_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		if (shell->is_interactive)
			ms_setup_interactive_signals();
		line = ms_read_line(shell);
		if (!line)
			break ;
		if (shell->is_interactive && line[0] != '\0')
			add_history(line);
		shell->current_line = line;
		ms_handle_line(shell, line);
		free(line);
		shell->current_line = NULL;
		shell->input_line_num++;
	}
}
```

So the flow is:

`main()`
→ `ms_detect_interactive()`
→ `ms_main_loop()`
→ `ms_read_line()`
→ `readline(PROMPT_STR)` shows `juma[n]she$ `
→ user types command
→ command is executed
→ loop repeats
→ prompt appears again

### 1.5) What happens on Ctrl+C

The prompt is also refreshed cleanly when the user presses `Ctrl+C`.

In `src/shell/init.c:57`:

```c
rl_signal_event_hook = ms_rl_event_hook;
```

And in `src/shell/signals.c:22-31`:

```c
int	ms_rl_event_hook(void)
{
	if (g_signal_number == SIGINT)
	{
		g_signal_number = 0;
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	return (0);
}
```

This is what makes Readline redraw a fresh prompt after an interrupt.

#### Ctrl+C redraw flow

When you press `Ctrl+C` at the prompt, this is the refresh path:

```text
SIGINT
 └─ ms_sigint_interactive()
     ├─ g_signal_number = SIGINT
     └─ write("\n")
 
Readline hook runs:
 └─ ms_rl_event_hook()
     ├─ rl_on_new_line()
     ├─ rl_replace_line("", 0)
     └─ rl_redisplay()
          └─ fresh prompt is shown again
```

That is why the shell returns to a clean prompt instead of exiting.

### 1.6) In one sentence

Our minishell displays the prompt by detecting interactive mode with `isatty(...)`, then calling `readline(PROMPT_STR)` inside the main loop each time it is ready to read a new command.

Here is a tiny call-flow diagram from `main()` to `readline()`:

```text
main()
 ├─ ms_init_shell(&shell, envp)
 │   └─ rl_signal_event_hook = ms_rl_event_hook
 │
 ├─ ms_detect_interactive(&shell)
 │   └─ if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
 │         shell.is_interactive = 1
 │      else
 │         shell.is_interactive = 0
 │
 └─ ms_main_loop(&shell)
     └─ while (!shell->should_exit)
         ├─ if (shell->is_interactive)
         │    └─ ms_setup_interactive_signals()
         │
         ├─ line = ms_read_line(shell)
         │    ├─ interactive:
         │    │    └─ readline(PROMPT_STR)
         │    │         ├─ displays: "juma[n]she$ "
         │    │         └─ waits for user input
         │    │
         │    └─ non-interactive:
         │         └─ ms_get_next_line(STDIN_FILENO)
         │              └─ no prompt shown
         │
         ├─ if line not empty and interactive
         │    └─ add_history(line)
         │
         ├─ ms_handle_line(shell, line)
         │    ├─ lex
         │    ├─ parse
         │    └─ execute
         │
         └─ loop repeats
              └─ prompt appears again on next readline(PROMPT_STR)
```

#### The important part

The normal shell prompt is not printed manually with `write()`.

It is displayed here in `src/shell/loop.c`:

```c
line = readline(PROMPT_STR);
```

So `readline()` is doing both jobs:

* showing the prompt
* waiting for the next command

#### Prompt text source

In `include/minishell.h`:

```c
# define PROMPT_STR "juma[n]she$ "
```

#### Why the prompt only appears in terminal mode

In `src/shell/main.c`:

```c
if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
	shell->is_interactive = 1;
```

So:

```text
./minishell
```

shows:

```text
juma[n]she$
```

but:

```text
echo "ls" | ./minishell
```

does not show a prompt, because it reads from stdin non-interactively.

---
### One-line summary

Our minishell displays the “waiting for a new command” prompt by calling `readline(PROMPT_STR)` inside `ms_main_loop()`, but only when `isatty()` says the shell is running interactively.

---
## 2) heredoc prompt is different
Important distinction: The normal command prompt uses `readline(PROMPT_STR)`, but the heredoc prompt does **not**.

<<<<<<< HEAD
### Separate case: heredoc prompt

Our heredoc prompt is different from the normal shell prompt.

Normal prompt:
=======
In `include/minishell.h:33`:
>>>>>>> docs

```c
# define HEREDOC_PROMPT "> "
```

And in `src/heredoc/heredoc_read.c:19-25`:

```c
if (shell->is_interactive)
{
	write(STDERR_FILENO, HEREDOC_PROMPT, ft_strlen(HEREDOC_PROMPT));
	line = ms_get_next_line(STDIN_FILENO);
	...
}
```

So:

* **new command prompt** → `readline(PROMPT_STR)`
* **heredoc prompt** → manual `write("> ")`

So:

```text
juma[n]she$ cat << EOF
> hello
> EOF
```

comes from two separate mechanisms:

* `juma[n]she$ ` → `readline(PROMPT_STR)`
* `> ` → manual `write(HEREDOC_PROMPT, ...)`

---

<<<<<<< HEAD
Our minishell displays the “waiting for a new command” prompt by calling `readline(PROMPT_STR)` inside `ms_main_loop()`, but only when `isatty()` says the shell is running interactively.
=======
### 2.1) Is there a reason to use `ms_get_next_line`?

**Pros:**

* Very simple input loop
* No readline side-effects (signals, history, prompt handling)
* Easy to control exact behavior
* No need to deal with readline quirks in child processes

**Cons:**

* No prompt like `> `
* No line editing
* No arrow keys
* No Ctrl+D formatting (must be handled manually)
* No Ctrl+C handling “for free”

This is why additional handling is required:

* manually printing a newline
* manually handling signals
* manually fixing Ctrl+\ behavior

---

### 1.2) Could we use `readline` instead?

Yes — we could replace:

```
line = ms_get_next_line(STDIN_FILENO);
```

with:

```c
line = readline("> ");
```


**But:**

__*1.2.1) History*__

The subject states that heredoc **does not have to update history**.

So we must **not** call:

```c
add_history(line);
```

---
__*1.2.2) Signals*__

`readline` already handles:

* Ctrl+C
* Ctrl+D
* prompt redisplay

However, in a child process, **readline signal behavior can become unpredictable** unless carefully configured.


### 1.3) Why we decicded not to use `readline` in heredoc

`readline` is designed for interactive shells, not child processes.

Common issues include:

* signal duplication
* prompt glitches
* double newlines
* internal readline state conflicts

---

### 1.4) Our current design

* fork a heredoc child
* read lines using `ms_get_next_line`
* write to a temporary file (`~/tmp/...`)

is clean and robust.

---
### 1.5) Comparison

| Feature            | GNL    | Readline               |
| ------------------ | ------ | ---------------------- |
| Simplicity         | Yes    | No                     |
| Control            | Yes    | Less                   |
| Signals            | Manual | Automatic (but tricky) |
| Prompt `> `        | Manual | Built-in               |
| Stability in child | Yes    | Potential issues       |
| Subject compliance | Yes    | Yes                    |

---
>>>>>>> docs
