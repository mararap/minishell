**working history is achieved by using the Readline history API**, not by a custom linked list or array in our own code.

## Core idea

Our shell does two things:

1. **reads commands with `readline()`**
2. **stores each non-empty interactive command with `add_history()`**

After that, **Readline itself** handles the usual history behavior:

* Up arrow → older command
* Down arrow → newer command
* editable recalled line

So the “history feature” is mostly provided by the `readline` library, and our code just feeds commands into it.

---

## Where this is set up

### 1) Readline history is included

In `include/minishell.h`:

```c
# include <readline/history.h>
# include <readline/readline.h>
```

That gives access to:

* `readline()`
* `add_history()`
* `rl_clear_history()`

### 2) The project links against Readline

In our `Makefile`:

```make
LDLIBS := -lft -lreadline
```

So the compiled shell uses the GNU Readline library.

---

## Where commands are added to history

The key logic is in `src/shell/loop.c`:

```c
if (shell->is_interactive && line[0] != '\0')
	add_history(line);
```

This means a command is stored in history only when:

* the shell is running **interactively**
* the line is **not empty**

So:

* `ls` → added
* `echo hello` → added
* just pressing Enter on an empty line → **not** added

---

## Why arrow-key history works

Because input is read with:

```c
line = readline(PROMPT_STR);
```

from the same file: `src/shell/loop.c`.

This is the important part many people miss:

* `add_history(line)` only stores the line
* `readline()` is what later lets the user browse that stored history with arrow keys

So our code does **not** manually implement:

* previous/next command navigation
* line recall
* editing a recalled command

Readline handles all of that automatically.

---

## Full flow

```text
main()
 └─ ms_main_loop(shell)
     └─ ms_read_line(shell)
         └─ readline(PROMPT_STR)
              └─ user types a command
     └─ if interactive and non-empty
         └─ add_history(line)
     └─ execute command
     └─ next iteration
         └─ readline() can now recall earlier commands
```

So once a command has been passed to `add_history()`, the next `readline()` call can access it.

---

## Interactive mode matters

In `src/shell/main.c`, our shell detects whether it is attached to a terminal:

```c
if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
	shell->is_interactive = 1;
```

Because of that, history is only active in normal terminal usage.

So:

```bash
./minishell
```

has history.

But:

```bash
echo "ls" | ./minishell
```

does not build interactive history, because that is not an interactive session.

---

## What happens on exit

In `src/shell/init.c`:

```c
rl_clear_history();
```

inside `ms_free_shell()`.

That clears Readline’s internal history list and frees its memory when the shell exits.

So the history is cleaned up properly at shutdown.

---

## Important limitation: history is only for the current session

There is **no** call to:

* `read_history(...)`
* `write_history(...)`

That means our shell **does not save history to a file** between runs.

So this works:

```text
run minishell
type: ls
type: pwd
press Up -> pwd
press Up -> ls
```

But after exiting and starting minishell again, that old history is gone.

So our project has:

* **session history** ✅
* **persistent history across restarts** ❌

For 42 minishell, session history is typically what “working history” means.

---

## Heredoc is separate

Our heredoc input does **not** use `readline()` for history recall.

In `src/heredoc/heredoc_read.c`, heredoc lines are read with:

```c
line = ms_get_next_line(STDIN_FILENO);
```

and not added with `add_history()`.

So heredoc text is not part of the shell command history.

Example:

```bash
cat << EOF
hello
world
EOF
```

Only the command itself may appear in history, not the inner heredoc lines.

---

## One-line summary

Our minishell achieves “working history” by using **Readline’s built-in history system**: commands are read with `readline()`, stored with `add_history()` when interactive and non-empty, and later recalled automatically by Readline with the arrow keys.
