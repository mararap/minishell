Here’s how `export` is implemented in your `minishell.zip`.

## Why `export` is special

`export` changes the shell’s environment, so it must run in the **parent shell process**, not in a child.

Your project handles that correctly:

* `export` is recognized as a builtin in `src/builtin/builtin_is_builtin.c`
* it is marked as a builtin that **needs the parent** in `src/builtin/builtins.c`
* when it is a single builtin command, the executor runs it through `ms_run_builtin_parent(...)`

So changes made by `export` persist in the shell.

---

## Dispatch path

In `src/builtin/builtins.c`:

```c
if (ft_strncmp(argv[0], "export", 7) == 0)
	return (ms_builtin_export(shell, argv));
```

And it is marked as parent-only here:

```c
if (ft_strncmp(cmd_name, "export", 7) == 0)
	return (1);
```

So plain `export ...` really updates `shell->env_list`.

---

## The main builtin function

The real entry point is:

```c
int	ms_builtin_export(t_shell *shell, char **argv)
```

from `src/builtin/builtin_export.c`.

Its logic is simple:

### 1. No arguments

If `export` is called alone:

```c
if (!argv[1])
{
	ms_print_export_format(shell->env_list);
	return (0);
}
```

So `export` with no arguments prints the exported variables.

### 2. With arguments

If there are arguments, it processes them one by one:

```c
while (argv[i])
{
	if (ms_export_one_arg(shell, argv[i]))
		exit_code = 1;
	i++;
}
```

So it supports:

* `export NAME`
* `export NAME=value`

That is broader than the minimal requirement, but correct shell behavior.

---

## How `export` with no arguments prints the environment

This part is in `src/builtin/builtin_export_print.c`.

### Step 1: copy env nodes into an array

`ms_export_sorted_env()` walks the linked list and builds an array of pointers.

### Step 2: sort alphabetically

`ms_sort_env_array()` sorts by variable name:

```c
if (ft_strcmp(arr[i]->name, arr[j]->name) > 0)
```

So the output is printed in lexicographic order.

### Step 3: print only exported entries

`ms_print_export_format()` prints entries only when:

```c
arr[i]->exported != 0
```

and it skips `_`:

```c
if (ft_strcmp(arr[i]->name, "_") != 0 && arr[i]->exported != 0)
```

### Step 4: format

Each line is printed like:

```c
export NAME="value"
```

or, if the value is `NULL`:

```c
export NAME
```

That is done in `ms_print_export_entry()`.

So `export` alone behaves like:

> show the shell’s exported variables in sorted `export ...` format

---

## How one export argument is handled

That logic is in `src/builtin/builtin_export_utils.c`, in:

```c
int	ms_export_one_arg(t_shell *shell, char *arg)
```

It does 4 things:

### 1. Split `NAME=value` if `=` exists

`ms_parse_export_pair()` checks for `=`.

* if there is no `=`, it treats the whole argument as `name`
* if there is `=`, it splits into:

  * `name`
  * `value`

So:

* `export PATH=/bin`
* `export HOME=/tmp`
* `export TEST`

are all supported.

---

### 2. Validate the identifier

`ms_is_valid_identifier()` allows:

* first character: letter or `_`
* remaining characters: alnum or `_`

So these are valid:

* `TEST`
* `_ABC`
* `VAR123`

These are invalid:

* `1ABC`
* `A-B`
* `A+B`

If invalid, it prints:

```c
minishell: export: `ARG': not a valid identifier
```

through `ms_export_error()`.

---

### 3. If `NAME=value`, set/update the variable

When there is a value:

```c
status = ms_env_set(&shell->env_list, name, value, 1);
```

So the variable is:

* created if missing
* updated if already present
* marked as exported (`1`)

---

### 4. If only `NAME`, mark it exported

When there is no `=`:

```c
status = ms_export_name_only(shell, name);
```

That function does:

* look up the old value
* if not found, use empty string fallback
* call `ms_env_set(..., exported = 1)`

So `export NAME` means:

> ensure the variable exists in the shell env list and mark it exported

---

## How the environment is actually stored

Your shell uses a linked list of `t_env_var`.

The actual setter is in `src/env/env_list_ops.c`:

```c
int	ms_env_set(t_env_var **env_list, char *name, char *value, int exported)
```

It works like this:

### If variable already exists

```c
if (var)
{
	free(var->value);
	var->value = ms_strdup_safe(value);
	var->exported = exported;
	return (0);
}
```

So it updates the existing node.

### If variable does not exist

It creates a new node and appends it to the env list.

So `export TEST=42` permanently updates `shell->env_list`.

---

## Important detail: exported flag

Each env node has an `exported` field.

That is why your shell can distinguish between:

* variables present in the shell
* variables that should appear in exported output / child `envp`

`export` sets that flag to `1`.

---

## One concrete example

For:

```bash
export TEST=hello
```

your shell does:

1. recognize builtin `export`
2. run it in the parent
3. `ms_export_one_arg("TEST=hello")`
4. split into:

   * `name = "TEST"`
   * `value = "hello"`
5. validate `TEST`
6. call:

```c
ms_env_set(&shell->env_list, "TEST", "hello", 1);
```

So now `TEST` is in the shell environment and marked exported.

Then:

```bash
export
```

will include something like:

```text
export TEST="hello"
```

---

## One more example

For:

```bash
export ABC
```

your shell does:

1. parse no `=`
2. validate `ABC`
3. call `ms_export_name_only()`
4. if `ABC` did not exist, it creates it with empty-string fallback
5. marks it exported

So `export ABC` does not assign a value, but it still creates/exports the name in your shell’s env list.

---

## Bottom line

Your `export` implementation works like this:

* it is a **parent builtin**
* `export` with no arguments prints the exported environment in sorted `export NAME="value"` format
* `export NAME=value` sets or updates a variable
* `export NAME` marks a variable as exported
* invalid identifiers are rejected with an error
* all updates go through `ms_env_set(...)` on `shell->env_list`

The most important functions are:

* `ms_builtin_export()` — main entry point
* `ms_print_export_format()` — `export` with no args
* `ms_export_one_arg()` — handles one assignment/name
* `ms_env_set()` — updates the shell environment

Next is `unset`.
