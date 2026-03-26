Here’s how `unset` is implemented in your `minishell.zip`.

## Why `unset` is special

`unset` modifies the shell’s own environment, so it has to run in the **parent shell process**.

Your project handles that correctly:

* `unset` is recognized as a builtin
* it is marked as a builtin that **needs the parent**
* when it is a single builtin command, the executor runs it with `ms_run_builtin_parent(...)`

So removing a variable with `unset` really changes `shell->env_list` for later commands.

---

## Where `unset` is dispatched

In `src/builtin/builtins.c`:

```c
if (ft_strncmp(argv[0], "unset", 6) == 0)
	return (ms_builtin_unset(shell, argv));
```

And it is marked as parent-only here:

```c
if (ft_strncmp(cmd_name, "unset", 6) == 0)
	return (1);
```

So plain `unset NAME` runs in the shell process, not in a child.

---

## The builtin itself is very small

In `src/builtin/builtin_unset.c`:

```c
int	ms_builtin_unset(t_shell *shell, char **argv)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		ms_env_unset(&shell->env_list, argv[i]);
		i++;
	}
	return (0);
}
```

So `unset` works like this:

1. skip `argv[0]` (`"unset"`)
2. for each remaining argument
3. call `ms_env_unset(&shell->env_list, name)`

That means it supports multiple names at once:

```bash
unset PATH HOME TEST
```

and removes them one by one.

---

## How the actual removal works

The real environment deletion logic is in `src/env/env_list_unset.c`:

```c
int	ms_env_unset(t_env_var **env_list, char *name)
{
	t_env_var	*iter;
	t_env_var	*prev;

	if (!name || !ms_env_is_valid_name(name))
		return (1);
	iter = *env_list;
	prev = NULL;
	while (iter)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			if (!prev)
				*env_list = iter->next;
			else
				prev->next = iter->next;
			free(iter->name);
			free(iter->value);
			free(iter);
			return (0);
		}
		prev = iter;
		iter = iter->next;
	}
	return (0);
}
```

This function does three important things.

### 1) Validate the identifier

Before touching the list, it checks:

```c
if (!name || !ms_env_is_valid_name(name))
	return (1);
```

And `ms_env_is_valid_name()` in `src/env/env_list.c` requires:

* first char = letter or `_`
* rest = alnum or `_`

So valid names are things like:

* `PATH`
* `HOME`
* `_TEST`
* `VAR123`

Invalid names are things like:

* `1ABC`
* `A-B`
* `A+B`

---

### 2) Walk the linked list

It traverses `shell->env_list` with:

* `iter` = current node
* `prev` = previous node

So it can unlink the matching variable cleanly.

---

### 3) Unlink and free the node

When it finds the matching variable name:

```c
if (!prev)
	*env_list = iter->next;
else
	prev->next = iter->next;
free(iter->name);
free(iter->value);
free(iter);
```

So removal is done correctly whether the variable is:

* the first node in the list
* a middle node
* the last node

That is the actual “unset” operation.

---

## What happens if the variable does not exist

If the name is valid but not found in the list, `ms_env_unset()` just returns `0`.

So this is harmless:

```bash
unset DOES_NOT_EXIST
```

No error is printed, and the shell continues normally.

That is good shell behavior.

---

## What “no options” looks like in this implementation

Your current `unset` does **not** have option parsing at all.

It simply treats every argument after `unset` as a variable name to remove.

So the intended valid usage is:

```bash
unset NAME
unset NAME1 NAME2
```

### Important nuance

If the user writes something like:

```bash
unset -x
```

then `"-x"` is passed to `ms_env_unset()` as a name.

`ms_env_unset()` sees that it is not a valid identifier and returns `1`, but `ms_builtin_unset()` ignores that return value and still finishes with:

```c
return (0);
```

So your implementation does not explicitly reject options with an error message.
It just attempts to unset each argument as a variable name.

So the core requirement is satisfied, but the builtin is a bit looser than a strict Bash-style option check.

---

## One concrete example

For:

```bash
unset PATH
```

your shell does:

1. recognize `unset` as a parent builtin
2. call `ms_builtin_unset(shell, argv)`
3. loop to `argv[1] = "PATH"`
4. call:

```c
ms_env_unset(&shell->env_list, "PATH");
```

5. walk the env list
6. find the `PATH` node
7. unlink it and free it

After that:

* `$PATH` expansion no longer finds a value
* external command lookup no longer has the same `PATH` behavior

So `unset PATH` really changes future shell behavior.

---

## Another example

For:

```bash
unset HOME OLDPWD TEST
```

the builtin loops through all three names and removes each one separately.

That is why one `unset` command can affect several variables at once.

---

## Bottom line

Your `unset` implementation works by:

* running as a **parent builtin**
* looping over every argument after `unset`
* calling `ms_env_unset()` on `shell->env_list`
* validating the variable name
* unlinking and freeing the matching environment node if found

The key line is:

```c
ms_env_unset(&shell->env_list, argv[i]);
```

and inside that helper, the actual delete is the linked-list unlink + `free(...)`.

Next is `env`.
