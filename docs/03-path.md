Searching and launching the right executable is done by a small chain of execution functions.

## The actual call flow

For a normal external command, your code goes through this path:

```text
ms_handle_line()
  -> ms_execute_commands()
    -> ms_execute_pipeline()
      -> ms_spawn_pipeline()
        -> ms_fork_and_execute()
          -> ms_execute_child()
            -> ms_exec_external_command()
              -> ms_find_executable()
              -> ms_exec_precheck()
              -> execve()
```

So the real “find the command and run it” logic lives mostly in:

* `src/exec/exec_path.c`
* `src/exec/exec_run.c`
* `src/exec/exec_error.c`
* `src/exec/fork.c`

---

## 1) Builtins are filtered out first

In `src/exec/fork.c`, once the child is set up and redirections are applied, our shell decides whether the command is a builtin or an external executable:

```c
if (ms_is_builtin(cmd->argv[0]))
	ms_child_exit(ctx, ms_run_builtin_child(ctx->shell, cmd->argv));
status = ms_exec_external_command(ctx->shell, cmd->argv);
```

So only **non-builtins** go into the executable-search logic.

That means:

* `cd`, `echo`, `pwd`, `export`, `unset`, `env`, `exit` → builtin path
* `ls`, `cat`, `grep`, `./a.out`, `/bin/ls` → external-command path

---

## 2) `ms_find_executable()` decides how the command should be resolved

The key function is in `src/exec/exec_path.c`:

```c
char	*ms_find_executable(t_shell *shell, char *cmd, int *used_path)
```

This function applies the shell’s resolution rules.

### Case A: command contains `/`

This line is the big switch:

```c
if (ft_strchr(cmd, '/'))
	return (ft_strdup(cmd));
```

If the command contains a slash, our minishell **does not search PATH**.

So all of these are treated as explicit paths:

* `/bin/ls` → absolute path
* `./minishell` → relative path
* `../prog` → relative path
* `subdir/tool` → relative path

Our shell just duplicates that string and tries to execute it as given.

---

### Case B: command does not contain `/`

If there is no slash, our shell looks up `PATH`:

```c
path_env = ms_env_get_value(shell->env_list, "PATH");
```

Then:

```c
paths = ft_split(path_env, ':');
return (ms_search_path_dirs(paths, cmd, used_path));
```

So a command like:

```bash
ls
```

turns into a search through each directory in `PATH`.

Example:

```text
PATH=/usr/local/bin:/usr/bin:/bin
cmd=ls
```

Search order becomes roughly:

```text
/usr/local/bin/ls
/usr/bin/ls
/bin/ls
```

The first match wins.

---

## 3) How each PATH directory is searched

The search loop is in `ms_search_path_dirs()`:

```c
candidate = ms_join_search_dir(paths[i], cmd);
if (access(candidate, F_OK) == 0 && stat(candidate, &st) == 0
	&& !S_ISDIR(st.st_mode))
{
	...
	return (candidate);
}
```

Let’s unpack what’s happening step by step and connect it to how a real shell behaves.

---

### 3.1) Big picture: what this loop is doing

When the user types a command like:

```bash
ls
```

our shell doesn’t know where `ls` is located. It uses the `PATH` environment variable, which might look like:

```bash
PATH=/usr/local/bin:/usr/bin:/bin
```

The function `ms_search_path_dirs()` is essentially doing:

👉 “Try each directory in `PATH`, one by one, and see if `cmd` exists there.”

---

### 3.2) The core loop logic

Here’s the important part again:

```c
candidate = ms_join_search_dir(paths[i], cmd);
if (access(candidate, F_OK) == 0 && stat(candidate, &st) == 0
    && !S_ISDIR(st.st_mode))
{
    ...
    return (candidate);
}
```

Let’s break this into **clear steps**.

---

### 3.3) Step-by-step execution

#### Step 1: Build a candidate path

```c
candidate = ms_join_search_dir(paths[i], cmd);
```

This creates:

```
candidate = directory + "/" + command
```

Example:

| PATH entry (`paths[i]`) | cmd | candidate     |
| ----------------------- | --- | ------------- |
| `/usr/bin`              | ls  | `/usr/bin/ls` |
| `/bin`                  | ls  | `/bin/ls`     |

---

#### Step 2: Check if it exists

```c
access(candidate, F_OK) == 0
```

* `F_OK` means: *“Does this file exist?”*
* Returns `0` → YES, it exists
* Returns `-1` → NO

So this filters out non-existent paths.

---

#### Step 3: Get file metadata

```c
stat(candidate, &st) == 0
```

* This retrieves file information into `st`
* If it fails → skip this candidate

---

#### Step 4: Reject directories

```c
!S_ISDIR(st.st_mode)
```

* Ensures the file is **not a directory**
* Because you cannot execute a directory

---

#### Step 5: Return first match

As soon as one valid candidate is found:

```c
return (candidate);
```

👉 The search **stops immediately** (just like a real shell)

---

### 3.4) Important missing check (real shells do this)

Our code checks:

* exists ✅
* not directory ✅

But a real shell also checks:

```c
access(candidate, X_OK)
```

👉 This ensures the file is **executable**

Otherwise, we'd accept files that exist but cannot be run.

---

### 3.5) Special case: empty PATH entries

This is the subtle but important part:

```c
if (dir[0] == '\0')
    return (ms_str_join_three(".", "/", cmd));
```

#### What does this mean?

If a PATH entry is **empty**, like:

```bash
PATH=:/bin
```

Splitting by `:` gives:

```
["", "/bin"]
```

The first entry is `""` (empty string).

---

#### How your code handles it

Instead of:

```
"" + "/" + cmd  → "/cmd"   ❌ (wrong)
```

You do:

```
"." + "/" + cmd → "./cmd"  ✅
```

---

### 3.6) Why this matters (real shell behavior)

In POSIX shells:

```bash
PATH=:/bin
```

means:

1. First search in **current directory (`.`)**
2. Then search in `/bin`

So typing:

```bash
myprog
```

will try:

```
./myprog
/bin/myprog
```

---

### 3.7) More examples of PATH behavior

#### Example 1

```bash
PATH=/usr/bin::/bin
```

Split:

```
["/usr/bin", "", "/bin"]
```

Search order:

```
/usr/bin/cmd
./cmd
/bin/cmd
```

---

#### Example 2

```bash
PATH=:
```

Split:

```
["", ""]
```

Search:

```
./cmd
./cmd
```

(yes, current directory twice)

---

### 3.8) Subtle security implication

Empty PATH entries mean **current directory is searched automatically**.

That’s dangerous because:

```bash
./ls   # malicious file
```

could override real `/bin/ls`.

That’s why many systems avoid putting `.` in `PATH`.

---

### 3.9) Summary

Our function is doing exactly this:

1. Loop over each PATH directory
2. Build:

   ```
   dir + "/" + cmd
   ```
3. Check:

   * file exists
   * `stat()` works
   * not a directory
4. Return first match
5. Special case:

   * empty PATH entry → `"./cmd"`

---

So for each PATH entry, our shell builds:

```text
dir + "/" + cmd
```

and accepts it if:

* it exists
* `stat()` works
* it is **not** a directory

### Important detail: empty PATH entries mean current directory

This helper shows it:

```c
if (dir[0] == '\0')
	return (ms_str_join_three(".", "/", cmd));
```

So if `PATH` contains an empty component, like:

```bash
PATH=:/bin
```

then the empty part is treated as:

```text
./cmd
```

which matches standard shell behavior for an empty PATH element.

---

## 4) What happens when PATH is missing or empty

This part is subtle and important:

```c
if (!path_env || path_env[0] == '\0')
	return (ft_strdup(cmd));
```

So if `PATH` is unset or empty, your shell does **not** search directories.

Instead, it just returns the raw command name unchanged.

So:

```bash
ls
```

with empty PATH becomes effectively:

```text
try to execute "ls" directly
```

not:

```text
search /bin/ls, /usr/bin/ls, ...
```

That is why in such cases you tend to get errors like:

```text
ls: No such file or directory
```

instead of a PATH-based lookup result.

### One more nuance from our init code

In `src/shell/init.c`, our shell bootstraps a default PATH if PATH is missing at startup:

```c
if (!ms_env_get_value(shell->env_list, "PATH"))
	ms_env_set(&shell->env_list, "PATH",
		"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 0);
```

So if minishell starts with no PATH in the environment, it creates one.
But if the user later does `unset PATH` or `export PATH=""`, then `ms_find_executable()` follows the “no PATH search” behavior above.

---

## 5) What if nothing is found in PATH

If `ms_search_path_dirs()` fails to find any candidate, it returns `NULL`.

Then `ms_exec_external_command()` does:

```c
if (!path)
{
	ms_print_command_not_found(argv[0]);
	return (127);
}
```

So this is the true “command not found” path.

Example:

```bash
nosuchcmd
```

with a normal PATH:

* search every PATH directory
* no match found
* print `nosuchcmd: command not found`
* return exit status `127`

---

## 6) Pre-check before `execve()`

Once a path is found or accepted, our code does a safety pre-check in `src/exec/exec_error.c`:

```c
status = ms_exec_precheck(argv[0], display_arg, path);
```

Inside:

```c
if (stat(path, &file_info) == -1)
	return (ms_exec_error_code(display_arg, errno));
if (S_ISDIR(file_info.st_mode))
	return (ms_exec_directory_status(argv0, display_arg));
return (-1);
```

So before launching, it checks:

* does `stat(path)` succeed?
* is it a directory?

If it is a directory:

* `.` is treated specially as `command not found`
* other directories produce `Is a directory`
* exit status becomes `126`

So:

```bash
/bin
```

or

```bash
./some_directory
```

will fail before `execve()` with the right directory-style error.

---

## 7) Actual launch happens with `execve()`

The real execution is in `src/exec/exec_run.c`:

```c
envp = ms_env_to_array(shell->env_list);
ms_adjust_envp_shlvl(envp);
execve(path, argv, envp);
```

That means our shell launches the chosen executable by calling:

* `path` = resolved executable path
* `argv` = parsed argument list
* `envp` = environment exported from your linked-list env

So once resolution is done, the kernel takes over and runs the program.

---

## 8) If `execve()` fails, your shell maps the error correctly

If `execve()` returns, execution failed. Then your code captures `errno` and converts it to shell-style messages and exit codes:

```c
err_no = errno;
status = ms_exec_error_code(display_arg, err_no);
```

Error mapping in `src/exec/exec_error.c`:

* `ENOENT` → `No such file or directory` → exit `127`
* `EISDIR` → `Is a directory` → exit `126`
* `ENOEXEC` → `Exec format error` → exit `126`
* `EACCES` → `Permission denied` → exit `126`

So even though PATH search only checks existence and “not a directory,” execute permission is effectively validated later by `execve()`.

That means a file found in PATH but not executable can still be selected, and then fail with:

```text
Permission denied
```

which is normal shell behavior.

---

## 9) Examples using our current code

### `ls`

* no slash
* search PATH
* first matching `.../ls` is selected
* `execve("/bin/ls", argv, envp)`

### `/bin/ls`

* contains slash
* no PATH search
* use `/bin/ls` directly
* `execve("/bin/ls", argv, envp)`

### `./myprog`

* contains slash
* no PATH search
* use `./myprog` directly

### `../tool`

* contains slash
* no PATH search
* use `../tool` directly

### `hello`

* no slash
* search PATH
* if not found anywhere → `hello: command not found`

### `hello` with `PATH=""`

* no slash
* PATH empty, so no search
* try literal `"hello"`
* likely `hello: No such file or directory`

### `PATH=:/bin` and command `hello`

* empty first PATH entry becomes `./hello`
* current directory is searched first
* then `/bin/hello`

---

## 10) One small but useful detail: `_` is updated

Our code updates `_` before and during execution:

* in `ms_execute_child()` it first sets `_` to `argv[0]`
* in `ms_exec_external_command()` it updates `_` again to the resolved `path`

So for external commands, `_` ends up tracking the actual executable path that was chosen.

---

## In one sentence

Our minishell searches and launches executables by sending non-builtins into `ms_exec_external_command()`, resolving commands with `/` as explicit paths, resolving slash-less commands by scanning `PATH`, rejecting directories, and finally launching the chosen file with `execve()`.
