Yes. In this `minishell.zip`, that requirement is achieved in a clean, standard way:

## 1) There is exactly one project-level global signal variable

In `src/shell/main.c:15`:

```c
volatile sig_atomic_t	g_signal_number = 0;
```

And in `include/minishell.h:44` it is only declared as:

```c
extern volatile sig_atomic_t	g_signal_number;
```

This matches the intent of the subject very closely:

* **one global variable**
* it stores only a **signal number**
* its type is **`volatile sig_atomic_t`**, which is the right type for communication between a signal handler and normal code

### Why this type matters

* `sig_atomic_t` is meant for values that can be safely read/written across signal interruptions
* `volatile` tells the compiler not to optimize reads/writes away, because the value may change asynchronously

You’re already on the right track—these two keywords solve *different but complementary problems* when dealing with signals. Let’s unpack *why the type actually matters*, not just what it does.

---

### 1.1. Why `sig_atomic_t` matters

Signals can interrupt your program **at literally any point**—even halfway through a variable update.

On many systems, writing to a variable is **not guaranteed to be atomic** unless the type explicitly promises it.

#### The problem without `sig_atomic_t`

Imagine this:

```c
int flag = 0;
```

If a signal handler does:

```c
flag = 1;
```

and the main program reads `flag`, the write might:

* happen in multiple CPU instructions
* get interrupted halfway
* leave `flag` in a **corrupted or inconsistent state**

This is especially possible on:

* larger-than-native-word types
* certain architectures
* optimized builds

#### What `sig_atomic_t` guarantees

```c
volatile sig_atomic_t flag = 0;
```

* Reads and writes happen **as a single, indivisible operation**
* No “half-written” values
* Safe to access from both:

  * normal code
  * signal handler

👉 In short: it protects against **data tearing** during asynchronous interruption.

---

### 1.2. Why `volatile` matters

Even if the operation is atomic, the compiler might still break your logic.

#### The problem without `volatile`

The compiler might optimize this:

```c
while (flag == 0) {
    // do nothing
}
```

into:

```c
if (flag == 0) {
    while (1) {}
}
```

Why? Because from the compiler’s perspective:

* nothing in the loop changes `flag`
* it assumes no external modification

But a **signal handler *can* change it**, and the compiler doesn’t know that unless you tell it.

#### What `volatile` guarantees

```c
volatile sig_atomic_t flag;
```

* Every read actually reloads from memory
* Every write actually stores to memory
* Prevents caching in registers
* Prevents “optimizing away” repeated reads

👉 In short: it protects against **incorrect compiler assumptions**.

---

### 1.3. Why you need BOTH

These solve *different layers of the problem*:

| Issue                           | Fixed by       |
| ------------------------------- | -------------- |
| Partial / torn reads-writes     | `sig_atomic_t` |
| Compiler caching / optimization | `volatile`     |

If you use only one:

* `sig_atomic_t` without `volatile` → compiler may ignore updates
* `volatile` without `sig_atomic_t` → you may get corrupted values

---

### 1.4. Subtle but important limitation

Even with:

```c
volatile sig_atomic_t flag;
```

you are only safe for:

* **simple reads and writes**

❌ NOT safe:

```c
flag++;   // read-modify-write → NOT atomic
```

Because that becomes:

1. read
2. modify
3. write

…and a signal can interrupt in between.

---

### 1.5. Mental model

Think of it like this:

* **`sig_atomic_t`** → “The hardware won’t tear this apart”
* **`volatile`** → “The compiler won’t pretend this never changes”

You need both because signals break assumptions at *two levels*:

* CPU execution (atomicity)
* compiler optimization (visibility)

---

If you want, I can show a real bug example where missing one of these causes an infinite loop or corrupted state—that usually makes it click instantly.


---

## 2) The main shell state is *not* global

Your shell state is kept in a local variable inside `main()`:

```c
t_shell	shell;
```

from `src/shell/main.c`.

That means things like:

* environment list
* exit status
* current input line
* interactive flag
* command state

all live inside `t_shell shell`, not in a global variable.

So the signal handler does **not** have direct access to:

* `t_shell *`
* token lists
* command lists
* env linked list
* parser/executor state

That is exactly what the subject wants to enforce.

---

## 3) The signal handlers themselves are minimal

### Interactive Ctrl+C handler

In `src/shell/signals.c`:

```c
static void	ms_sigint_interactive(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}
```

This handler does **not** touch any shell structure.
It only:

* stores `SIGINT` in the global flag
* writes a newline

### Heredoc Ctrl+C handler

In `src/heredoc/heredoc_tmp.c`:

```c
static void	ms_sigint_heredoc(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	rl_done = 1;
	write(STDOUT_FILENO, "\n", 1);
}
```

Again, it does not access `t_shell` or other project data structures.
It only:

* stores `SIGINT`
* tells Readline to stop (`rl_done = 1`)
* writes a newline

So the handlers are following the intended pattern:
**record the signal, then let normal code deal with the consequences later.**

---

## 4) The real reaction happens *outside* the signal handler

This is the most important part.

Instead of doing complex cleanup or state changes inside the handler, your program checks `g_signal_number` later in normal execution flow.

### Prompt redisplay after Ctrl+C

In `src/shell/signals.c`:

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

This is excellent for the project requirement, because:

* the handler only sets the flag
* the Readline hook later sees the flag
* only then does it redraw the prompt and clear the line

So UI/state repair is deferred to safe, normal code.

### Interrupted `get_next_line()` reads

In `src/utils/utils_gnl.c`:

```c
if (ret < 0 && errno == EINTR)
{
	if (g_signal_number == SIGINT)
		return (-1);
	continue ;
}
```

Again, the handler does not manipulate buffers or shell state.
It only sets `g_signal_number`, and the normal read loop reacts later.

### Heredoc loop

In `src/heredoc/heredoc_read.c`:

```c
if (g_signal_number == SIGINT)
	return (130);
```

So the heredoc logic also uses the same pattern:

* handler sets the signal number
* regular code sees it
* regular code decides to abort with status `130`

### Propagating heredoc interruption

In `src/heredoc/heredoc_prepare.c`:

```c
if (status == 130)
	g_signal_number = SIGINT;
```

This keeps the “signal state” represented by the same one global flag, instead of inventing extra global state.

---

## 5) Signal configuration is split by context, but still uses the same single flag

#### Interactive shell

In `src/shell/signals.c`:

```c
sa_int.sa_handler = ms_sigint_interactive;
sa_quit.sa_handler = SIG_IGN;
```

So:

* `SIGINT` is handled by setting the global flag
* `SIGQUIT` is ignored in the interactive shell

#### External child processes

Also in `src/shell/signals.c`:

```c
sa.sa_handler = SIG_DFL;
sigaction(SIGINT, &sa, NULL);
sigaction(SIGQUIT, &sa, NULL);
```

Child processes get default signal behavior again, which is correct for `execve()` children.

#### During pipeline execution

In `src/exec/executor.c`:

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

The parent shell ignores those signals while waiting for the pipeline, then restores interactive handlers later.

So the design is:

* shell parent: custom minimal handler + one signal flag
* child processes: default signals
* temporary waiting contexts: ignore/restore as needed

That separation helps keep the global flag simple.


You’re looking at a classic Unix shell design pattern, and it’s subtle but very intentional. Let’s break down what’s happening and *why* it’s done this way during pipeline execution.

---

### 5.1. The core problem: who should handle Ctrl+C?

When you press **Ctrl+C**, the terminal sends `SIGINT` to **every process in the foreground process group**:

* the shell (parent)
* all child processes in the pipeline (`ls | grep foo | wc`, etc.)

If the shell *also* reacts normally to `SIGINT`, you get problems:

* the shell might interrupt itself while it’s supposed to just wait
* it could print prompts mid-execution
* it might corrupt internal state (like your global flag)

So the shell must **step out of the way temporarily**.

---

### 5.2. What this code does

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

This tells the shell (parent process):

👉 “Ignore Ctrl+C and Ctrl+\ completely for now.”

This typically happens right before:

```c
waitpid(...);  // waiting for pipeline children
```

---

### 5.3. What happens to child processes?

When the shell forks children for the pipeline:

```c
if (fork() == 0) {
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    execve(...);
}
```

Each child restores **default behavior**:

* `SIGINT` → terminate
* `SIGQUIT` → terminate + core dump

So:

👉 **Children die on Ctrl+C — the shell does not.**

---

### 5.4. Example: `sleep 10 | cat`

#### Before execution:

* Shell has a custom handler (sets a global flag)

#### During execution:

* Shell ignores signals
* `sleep` and `cat` use default handlers

#### Press Ctrl+C:

* `sleep` → killed
* `cat` → killed
* shell → ignores it, keeps waiting

#### After children exit:

* shell regains control
* restores its normal interactive signal handler

---

### 5.5. Why ignore instead of handle?

You *could* handle `SIGINT` in the parent, but ignoring is safer here:

#### If the shell handled it:

* It might:

  * set flags unnecessarily
  * interrupt `waitpid`
  * require complex synchronization

#### By ignoring:

* The shell becomes **passive**
* Only children react
* `waitpid` continues cleanly

👉 This keeps your global signal flag untouched during execution.

---

### 5.6. The “three-layer design” you described

This is the key architectural idea:

#### 1. **Shell parent (interactive mode)**

* Custom handler:

  ```c
  void handler(int sig) {
      g_signal = sig;
  }
  ```
* Minimal logic
* Used only when reading input

---

#### 2. **Child processes**

* Reset to defaults:

  ```c
  signal(SIGINT, SIG_DFL);
  ```
* Behave like normal Unix programs

---

#### 3. **Temporary waiting context (pipeline execution)**

* Parent ignores signals:

  ```c
  signal(SIGINT, SIG_IGN);
  ```
* After waiting:

  ```c
  signal(SIGINT, handler); // restore
  ```

---

### 5.7. Why this separation is powerful

This design avoids a lot of headaches:

#### ✅ No race conditions

The global flag isn’t modified while executing children.

#### ✅ Clean responsibility split

* Parent = control flow
* Children = actual execution

#### ✅ Matches real shells

This is how shells like bash behave internally.

#### ✅ Predictable behavior

* Ctrl+C kills commands
* Shell survives
* Prompt comes back cleanly

---

### 5.8. What would go wrong without this?

If you *didn’t* ignore signals in the parent:

* Ctrl+C might:

  * interrupt `waitpid`
  * set your global flag mid-execution
  * cause double handling (parent + children)
* You’d need complicated logic like:

  * checking EINTR everywhere
  * resetting states manually

---

### 5.9. Mental model

Think of it like this:

* **Interactive phase** → shell listens to signals
* **Execution phase** → shell goes “deaf”
* **Children phase** → children “hear everything”

Then:

* execution ends
* shell “listens” again

---

### 5.10. Summary

Your design:

* **Parent shell**

  * minimal signal handler
  * ignores signals during execution

* **Children**

  * default signal behavior

* **Execution window**

  * temporary signal ignore + restore

👉 This keeps your global signal flag simple, avoids interference, and mirrors real shell behavior.

---

If you want, I can walk through a real `executor.c` flow (fork → pipe → waitpid) step by step with signal timing—that’s where this becomes crystal clear.


---

## 6) Why this satisfies the “no global structures” rule

The subject warns against things like:

* global `t_shell g_shell;`
* global structs containing pointers to commands/env/parser state
* global arrays/lists storing extra signal context
* global “Norm-friendly wrapper structs” that secretly expose application state

Your code avoids that.

I checked the project-level global definitions and the only one used for this purpose is:

```c
volatile sig_atomic_t g_signal_number
```

There is **no** global `t_shell`, no global executor state, no global parser state, and no global signal-context struct.

That is exactly the safe design the requirement is asking for.

---

## 7) The whole pattern in one flow

When the user presses `Ctrl+C` at the prompt:

```text
SIGINT arrives
-> ms_sigint_interactive()
-> g_signal_number = SIGINT
-> newline written

later, outside handler:
-> ms_rl_event_hook() sees SIGINT
-> clears current readline buffer
-> redraws prompt
-> resets g_signal_number to 0
```

When `Ctrl+C` happens in heredoc:

```text
SIGINT arrives
-> ms_sigint_heredoc()
-> g_signal_number = SIGINT
-> rl_done = 1
-> newline written

later, outside handler:
-> heredoc read loop sees g_signal_number == SIGINT
-> returns 130
-> parent handles heredoc interruption normally
```

---

## Bottom line

This requirement is achieved in your minishell by:

* defining exactly one global signal flag: `volatile sig_atomic_t g_signal_number`
* keeping `t_shell` and all real shell data out of global scope
* making handlers minimal and dumb
* deferring all meaningful logic to normal code that checks `g_signal_number`

So the handlers **signal what happened**, but they do **not** control the shell state directly.

A small extra note: your handlers also call `write()` and, for heredoc, set Readline’s `rl_done`. They still do not access your own shell data structures, so they respect the spirit of the 42 requirement very well.

Yes — in minishell terms:

* **`SIGINT`** is usually what you get from **Ctrl+C**
* **`SIGQUIT`** is usually what you get from **Ctrl+\\**
* **`sigaction()`** is the system call used to tell the OS **what your program should do when one of those signals arrives**

## 1) What is a signal?

A signal is a small asynchronous notification sent by the kernel to a process.

You can think of it as:

> “Something happened — react now.”

Examples:

* user pressed **Ctrl+C**
* user pressed **Ctrl+\\**
* a child process exited
* writing to a broken pipe happened

Signals can arrive at any moment, which is why handlers must stay very simple.

---

## 2) What is `SIGINT`?

`SIGINT` means **interrupt**.

In a terminal, the usual source is:

```text
Ctrl+C
```

Default behavior for most programs:

* terminate the process

### In a shell

A shell usually does **not** want Ctrl+C to kill the shell itself while it is sitting at the prompt.

Instead, when idle at the prompt, the shell usually wants:

* print a newline
* clear the current input line
* show a fresh prompt

That is exactly what your minishell does.

In `src/shell/signals.c`:

```c
static void	ms_sigint_interactive(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
}
```

So on Ctrl+C at the prompt, your shell:

* stores `SIGINT` in the global flag
* prints a newline

Then later, outside the handler:

```c
if (g_signal_number == SIGINT)
{
	g_signal_number = 0;
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}
```

That is what gives you the “clean new prompt” behavior.

---

## 3) What is `SIGQUIT`?

`SIGQUIT` means **quit**.

In a terminal, the usual source is:

```text
Ctrl+\
```

Default behavior is stronger than `SIGINT`:

* terminate the process
* often produce a **core dump**

That is why shells often treat it differently.

### In your interactive minishell

When waiting at the prompt, your shell ignores `SIGQUIT`:

```c
sa_quit.sa_handler = SIG_IGN;
```

So pressing Ctrl+\ at the prompt does nothing visible.

### But for child processes

Your shell resets child processes to default handling:

```c
sa.sa_handler = SIG_DFL;
sigaction(SIGINT, &sa, NULL);
sigaction(SIGQUIT, &sa, NULL);
```

So if you run an external command and press Ctrl+\, the child behaves like a normal Unix program.

Later, the parent shell checks how the child ended and prints:

```c
else if (sig == SIGQUIT)
	write(STDERR_FILENO, "Quit (core dumped)\n", 19);
```

That matches standard shell-style behavior.

---

## 4) What does `sigaction()` do?

`sigaction()` installs a signal disposition: it tells the kernel:

* which signal you care about
* what handler to run
* which extra flags to use
* which other signals to block while the handler runs

Basic shape:

```c
struct sigaction sa;

sa.sa_handler = my_handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;
sigaction(SIGINT, &sa, NULL);
```

Meaning:

> “When `SIGINT` arrives, call `my_handler`.”

---

## 5) Why use `sigaction()` instead of `signal()`?

`sigaction()` is the preferred modern POSIX interface because it is more explicit and reliable.

It gives you control over:

* `sa_handler` → function to run
* `sa_mask` → which signals to block during handler execution
* `sa_flags` → behavior options like `SA_RESTART`

Your code uses both:

* **`sigaction()`** for the main signal setup
* **`signal()`** in a few simpler places for quick ignore/default changes

That is common in student shells, though `sigaction()` is the more robust one.

---

## 6) What do the important `struct sigaction` fields mean?

In your code:

```c
struct sigaction	sa_int;
ft_bzero(&sa_int, sizeof(sa_int));
sa_int.sa_handler = ms_sigint_interactive;
sigemptyset(&sa_int.sa_mask);
sa_int.sa_flags = SA_RESTART;
sigaction(SIGINT, &sa_int, NULL);
```

### `sa_handler`

The function to call when the signal arrives.

Example:

```c
sa_int.sa_handler = ms_sigint_interactive;
```

### `sa_mask`

Signals to block while the handler is running.

You do:

```c
sigemptyset(&sa_int.sa_mask);
```

meaning:

* do not additionally block any extra signals

### `sa_flags`

Extra behavior options.

Your shell uses:

```c
sa_int.sa_flags = SA_RESTART;
```

for interactive `SIGINT`.

`SA_RESTART` means:

* if possible, interrupted system calls should be restarted automatically

That helps avoid some annoying partial interruptions while waiting for input.

---

## 7) How your minishell uses them in each context

### A) At the interactive prompt

In `ms_setup_interactive_signals()`:

* `SIGINT` → custom handler
* `SIGQUIT` → ignored

So:

* Ctrl+C → newline + fresh prompt
* Ctrl+\ → nothing

---

### B) In child processes

In `ms_setup_child_signals()`:

* `SIGINT` → default
* `SIGQUIT` → default

So external commands behave normally.

Example:

* running `cat`
* pressing Ctrl+C should stop `cat`, not kill the shell permanently

---

### C) While the parent waits for pipeline children

In `src/exec/executor.c`:

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

The parent ignores those signals temporarily while children are running.

Why?
Because the shell wants the foreground command to receive the signal, not the shell parent.

Then after waiting, your code restores interactive handlers.

---

### D) In heredoc

In `src/heredoc/heredoc_tmp.c`:

```c
static void	ms_sigint_heredoc(int signo)
{
	(void)signo;
	g_signal_number = SIGINT;
	rl_done = 1;
	write(STDOUT_FILENO, "\n", 1);
}
```

So Ctrl+C during heredoc:

* marks `SIGINT`
* tells Readline to stop
* prints newline
* later normal code aborts the heredoc cleanly

---

## 8) Why the global variable matters

Your shell uses:

```c
volatile sig_atomic_t g_signal_number = 0;
```

This is important because signal handlers should not manipulate your full shell state directly.

So instead of doing dangerous things like:

* editing linked lists
* freeing parser structures
* changing command arrays

the handler only says:

> “A `SIGINT` happened.”

Then the main code reacts later, safely.

That is exactly the right pattern for minishell.

---

## 9) What actually happens when you press Ctrl+C at the prompt?

Flow in your shell:

```text
Ctrl+C
-> kernel sends SIGINT
-> ms_sigint_interactive() runs
-> g_signal_number = SIGINT
-> newline printed

later:
-> ms_rl_event_hook() sees SIGINT
-> clear current line
-> redraw prompt
-> reset g_signal_number to 0
```

So the handler itself stays tiny, and the real UI cleanup happens afterward.

---

## 10) What happens when you press Ctrl+\ while running a command?

Example: run an external program.

Flow:

```text
child process has SIGQUIT = default
Ctrl+\
-> kernel sends SIGQUIT to foreground process
-> child quits
-> parent waitpid() sees child ended by SIGQUIT
-> shell prints "Quit (core dumped)"
```

That is why your shell behaves differently at the prompt versus inside a running command.

---

## 11) Tiny summary

* **`SIGINT`** = interrupt, usually **Ctrl+C**
* **`SIGQUIT`** = quit, usually **Ctrl+\**
* **`sigaction()`** = install precise signal behavior
* your minishell:

  * catches `SIGINT` at the prompt
  * ignores `SIGQUIT` at the prompt
  * restores default signal behavior in child processes
  * uses one global `g_signal_number` to remember which signal arrived

If you want, I can now explain this as a **table of behaviors**:

**prompt / child process / heredoc / pipeline wait** → what `SIGINT` and `SIGQUIT` do in each case.

Yes — here is the behavior matrix for **your** `minishell.zip`, based on the actual signal setup in the code.

## Signal behavior by context

| Context                                              | Where set                                                                                 | `SIGINT` (`Ctrl+C`)                        | `SIGQUIT` (`Ctrl+\`)                    | What the user sees                                                                                      |
| ---------------------------------------------------- | ----------------------------------------------------------------------------------------- | ------------------------------------------ | --------------------------------------- | ------------------------------------------------------------------------------------------------------- |
| **Interactive prompt**                               | `src/shell/signals.c:34-49` via `ms_setup_interactive_signals()`                          | custom handler `ms_sigint_interactive`     | ignored with `SIG_IGN`                  | `Ctrl+C` prints a newline, clears the current input, and redraws the prompt. `Ctrl+\` does nothing.     |
| **Child process after `fork()`**                     | `src/exec/fork.c:83-99` calls `ms_setup_child_signals()` from `src/shell/signals.c:51-61` | restored to default `SIG_DFL`              | restored to default `SIG_DFL`           | external commands behave like normal Unix programs: `Ctrl+C` kills them, `Ctrl+\` quits them.           |
| **Parent shell while waiting for pipeline children** | `src/exec/executor.c:37-43` in `ms_prepare_pipeline_signals()`                            | ignored with `signal(SIGINT, SIG_IGN)`     | ignored with `signal(SIGQUIT, SIG_IGN)` | the shell itself does not die while children are running; the foreground child gets the signal instead. |
| **After children finish**                            | `src/exec/executor_wait.c:24-35` and `:45-67`                                             | converts to exit status `128 + SIGINT`     | converts to exit status `128 + SIGQUIT` | for the last command: `SIGINT` prints a newline, `SIGQUIT` prints `Quit (core dumped)`                  |
| **Heredoc child**                                    | `src/heredoc/heredoc_tmp.c:71-86` via `ms_setup_heredoc_child_signals()`                  | custom heredoc handler `ms_sigint_heredoc` | ignored with `SIG_IGN`                  | `Ctrl+C` aborts the heredoc cleanly; `Ctrl+\` is ignored.                                               |
| **Heredoc parent while child is collecting input**   | `src/heredoc/heredoc_build.c:87-95`                                                       | ignored                                    | ignored                                 | parent waits; heredoc child handles the interrupt. Afterward the parent restores normal shell handlers. |

## What each case really means

### 1) Interactive prompt

At the prompt, your shell installs this behavior in `ms_setup_interactive_signals()`:

```c
sa_int.sa_handler = ms_sigint_interactive;
sa_int.sa_flags = SA_RESTART;
sa_quit.sa_handler = SIG_IGN;
sigaction(SIGINT, &sa_int, NULL);
sigaction(SIGQUIT, &sa_quit, NULL);
```

So:

* `Ctrl+C` does **not** kill minishell
* `Ctrl+\` is ignored

The handler itself is tiny:

```c
g_signal_number = SIGINT;
write(STDOUT_FILENO, "\n", 1);
```

Then later `ms_rl_event_hook()` checks `g_signal_number`, clears the line, and redraws the prompt.

So prompt behavior is:

```text
Ctrl+C -> newline -> empty prompt again
Ctrl+\ -> nothing
```

---

### 2) Child process after `fork()`

In `ms_execute_child()`:

```c
ms_setup_child_signals();
```

and that function sets both signals back to default:

```c
sa.sa_handler = SIG_DFL;
sigaction(SIGINT, &sa, NULL);
sigaction(SIGQUIT, &sa, NULL);
```

So once you are inside a child running a command, signals work like a normal process:

* `Ctrl+C` interrupts/kills it
* `Ctrl+\` quits it

That is why commands like `cat`, `grep`, `sleep`, etc. behave normally.

---

### 3) Parent shell while waiting for pipeline children

Before spawning a pipeline, the parent shell does:

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

in `ms_prepare_pipeline_signals()`.

That means the **parent shell ignores both signals temporarily** while children are active.

Why?

Because otherwise both the shell and the command could react to the same `Ctrl+C` or `Ctrl+\`.
Your code wants the **foreground command** to receive the signal, not the shell parent.

After the pipeline finishes, `ms_finish_pipeline()` restores interactive handlers with:

```c
ms_setup_interactive_signals();
```

---

### 4) How the shell reports child termination

When waiting, `ms_wait_for_children()` checks whether the last child died from a signal.

In `ms_report_child_signal()`:

* `SIGINT` → prints `\n`
* `SIGQUIT` → prints `Quit (core dumped)\n`

And the final shell status becomes:

```c
128 + WTERMSIG(status)
```

So:

* killed by `SIGINT` → exit status `130`
* killed by `SIGQUIT` → exit status `131`

That matches normal shell behavior.

---

### 5) Heredoc behavior

Heredoc is a little different.

When building a heredoc, the **parent** first ignores both signals:

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

then forks.

In the **heredoc child**, your code installs:

* `SIGINT` → custom handler
* `SIGQUIT` → ignored

The heredoc handler does:

```c
g_signal_number = SIGINT;
write(STDOUT_FILENO, "\n", 1);
```

and then the heredoc read loop notices:

```c
if (g_signal_number == SIGINT)
	return (130);
```

So:

* `Ctrl+C` aborts the heredoc
* the parent detects that and treats it as interrupted heredoc input
* `Ctrl+\` is ignored

Then `ms_restore_signals()` puts normal shell handlers back.

---

## Why `sigaction()` and `signal()` are both used

Your project uses both APIs, but for different roles:

### `sigaction()`

Used for the “real” signal setups:

* interactive shell
* child process defaults
* heredoc child

That is the more precise interface because it lets you control:

* handler
* mask
* flags like `SA_RESTART`

### `signal()`

Used as a quick shorthand when the code only wants to **temporarily ignore** or **restore defaults**:

* parent while pipeline children run
* parent while heredoc child runs
* non-interactive restore path

So in your code:

* **persistent/important setups** → `sigaction()`
* **temporary ignore/default switches** → `signal()`

---

## One subtle exception

A single builtin that must run in the parent, like `cd`, `export`, `unset`, `exit`, does **not** go through the forked child path. In that case it stays under the shell’s current parent-side signal behavior.

So the table above is most accurate for:

* prompt state
* forked children
* heredoc child
* parent waiting state

---

## The simplest mental model

Think of your minishell like this:

```text
At prompt:
  SIGINT -> custom
  SIGQUIT -> ignore

In child command:
  SIGINT -> default
  SIGQUIT -> default

In parent while waiting:
  SIGINT -> ignore
  SIGQUIT -> ignore

In heredoc child:
  SIGINT -> custom abort
  SIGQUIT -> ignore
```

That is the full signal strategy your project is using.

I can also draw this as a tiny timeline: **prompt -> fork -> child runs -> parent waits -> restore prompt handlers**.
