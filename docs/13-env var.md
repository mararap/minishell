Yes. In your `minishell.zip`, environment-variable expansion is done **during lexing**, before parsing and execution.

That is the key idea:

```text
raw input line
-> lexer builds TOKEN_WORD values
-> while building a word, '$NAME' is expanded
-> parser receives already-expanded word values
-> executor runs argv built from those values
```

## 1) Where variables live

Your shell stores the environment in `t_shell.env_list`, a linked list of `t_env_var` nodes from `include/minishell.h`.

At startup, `ms_init_shell()` fills that list from `envp`:

* `src/shell/init.c`
* `src/env/env_list.c`

And variable lookup is done by:

```c
char *ms_env_get_value(t_env_var *env_list, char *name);
```

implemented in `src/env/env_list_get.c`.

So when the shell expands `$HOME`, it is reading from the shell’s own environment list, not directly from `getenv()`.

---

## 2) The main expansion function

The real expansion logic is in:

* `src/expand/expand_variable.c`

```c
char	*ms_expand_variable(t_shell *shell, char *str, int *idx)
```

This function is called when the lexer sees a `$` in a place where expansion is allowed.

### What it handles

It supports:

* `$NAME`
* `$VAR_123`
* `$?`

### How it works

It checks:

#### `$?`

```c
if (str[*idx + 1] == '?')
	return (ft_itoa(shell->last_exit_status));
```

So `$?` expands to the previous command’s exit code.

#### Normal variable name

It scans characters after `$` while they are:

* alphanumeric
* `_`

```c
while (str[start] && (ft_isalnum(str[start]) || str[start] == '_'))
	start++;
```

Then it extracts the name, looks it up with `ms_env_get_value()`, and returns a duplicated value.

#### Missing variable

If the variable is not found:

```c
if (!value)
	return (ft_strdup(""));
```

So undefined variables expand to an empty string.

#### Bare `$` or invalid name after `$`

If there is no valid variable name after `$`, it returns literal `$`:

```c
if (start == *idx + 1)
{
	*idx = *idx + 1;
	return (ft_strdup("$"));
}
```

So things like `$-` do **not** become a variable lookup.
They become literal `$`, and the next character is processed afterward.

---

## 3) Where expansion is triggered

Expansion is not done later in execution.
It happens while the lexer is collecting a word.

That logic is in:

* `src/lexer/collect_word.c`

Inside `ms_collect_piece()`:

```c
if (wctx->str[*wctx->idx] == '$' && wctx->allow_expansion)
	return (ms_expand_variable(wctx->shell, wctx->str, wctx->idx));
```

So when the lexer is walking through a word and sees `$`, it immediately replaces that piece with the expanded value.

### Example

Input:

```bash
echo $HOME
```

Flow:

1. lexer starts collecting a word at `$HOME`
2. `ms_collect_piece()` sees `$`
3. calls `ms_expand_variable()`
4. gets something like `/home/jatanaso`
5. final token value becomes `/home/jatanaso`

So the parser never sees literal `$HOME`; it sees the expanded value.

---

## 4) Expansion depends on quoting context

This is one of the most important parts of your implementation.

### Unquoted words

Expansion is allowed.

Example:

```bash
echo $HOME
```

`$HOME` is expanded in `ms_collect_piece()`.

---

### Double quotes

Expansion is also allowed.

In `src/lexer/collect_word_quotes.c`, inside `ms_collect_dq_chunk()`:

```c
if (str[*idx] == '$' && allow_expansion)
	return (ms_expand_variable(shell, str, idx));
```

So inside `"..."`, `$VAR` still expands.

Example:

```bash
echo "$HOME"
```

expands `$HOME`.

---

### Single quotes

Expansion is **not** allowed.

`ms_collect_single_quotes()` just copies everything literally until the closing `'`.

It never calls `ms_expand_variable()`.

So:

```bash
echo '$HOME'
```

stays `$HOME`.

---

## 5) How spaces from expansions are handled

This is a subtle but very important part of your shell.

### Inside quotes

When text is collected inside quotes, your code masks spaces/tabs/newlines with hidden marker bytes using:

```c
ms_mask_ifs(tmp);
```

That happens in:

* `src/lexer/collect_word_quotes.c`

So if:

```bash
VAR="hello world"
echo "$VAR"
```

the expansion result keeps the internal space protected, so it stays **one argument**.

Later the parser restores those masked spaces with `ms_unmask_ifs()`.

Result:

```text
argv[1] = "hello world"
```

---

### Unquoted expansions

If an unquoted expansion contains spaces, your parser may split it into multiple arguments.

That happens in:

* `src/parser/handle_words.c`
* `src/expand/ifs_split.c`

In `ms_handle_word()`:

* if the word contains IFS whitespace
* it calls `ms_split_ifs_fields(tok->value)`

So:

```bash
VAR="a b c"
echo $VAR
```

can become:

```text
argv = ["echo", "a", "b", "c"]
```

But:

```bash
echo "$VAR"
```

stays:

```text
argv = ["echo", "a b c"]
```

That is a big part of correct shell behavior.

---

## 6) Empty expansions

Your code handles empty results carefully.

### Unquoted empty expansion

If a variable does not exist:

```bash
echo $NOT_SET
```

then `ms_expand_variable()` returns `""`.

Later, in `ms_handle_word()`, if the token value is empty and it was **not quoted**, it is dropped.

So unquoted empty expansions usually disappear as arguments.

---

### Quoted empty expansion

If it was quoted, your parser preserves it as an empty string argument:

```c
if ((tok->value[0]) == '\0')
{
	if (tok->quoted)
		cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));
}
```

So:

```bash
echo "$NOT_SET"
```

still passes an empty argument to `echo`, which is correct.

---

## 7) Expansion is done piece by piece inside a word

Your lexer does not require the whole word to be only a variable.
It collects and joins pieces.

In `ms_collect_word()`:

```c
tmp = ms_collect_piece(wctx);
ms_join_piece(&buf, tmp);
```

So a word like:

```bash
abc$HOME/xyz
```

is built from pieces:

* `abc`
* expanded value of `$HOME`
* `/xyz`

and joined into one final token value.

Example result:

```text
abc/home/jatanaso/xyz
```

That is exactly how shell words are normally built.

---

## 8) Special case: heredoc body expansion

Your project also has a separate expansion path for heredoc lines:

* `src/heredoc/heredoc_expand.c`

```c
char	*ms_expand_heredoc_line(t_shell *shell, char *line)
```

This also expands:

* `$VAR`
* `$?`

But only when the heredoc delimiter is unquoted.

So heredoc content uses the same general rule, but through a dedicated function.

---

## 9) One concrete walkthrough

Take:

```bash
export TEST="hello world"
echo X$TESTY "$TEST" '$TEST' $NOT_SET $?
```

Here is what your shell does:

### `X$TESTY`

The lexer sees `$TESTY` as one variable name, because it scans alnum and `_` after `$`.

So this looks up `TESTY`, not `TEST` + `Y`.

If `TESTY` is unset, it becomes:

```text
X
```

not `Xhello worldY`.

To get `TEST` followed by `Y`, your current minishell would need a separator, because it does **not** implement `${TEST}` syntax.

---

### `"$TEST"`

Expands inside double quotes, and the space is protected.

Result: one argv entry:

```text
hello world
```

---

### `'$TEST'`

Single quotes disable expansion.

Result:

```text
$TEST
```

---

### `$NOT_SET`

Undefined variable becomes `""`, then as unquoted empty text it disappears.

---

### `$?`

Expands to `shell->last_exit_status`.

---

## 10) A practical limitation of this implementation

Your shell supports the minishell-style form:

* `$NAME`
* `$?`

But it does **not** implement more advanced shell parameter syntax like:

* `${NAME}`
* `${NAME:-default}`
* `$1`, `$2`, etc. as shell positional parameters

That is because `ms_expand_variable()` only scans:

* alnum
* `_`

after `$`, plus the special case `$?`.

So it matches the basic minishell requirement, not full Bash parameter expansion.

---

## Bottom line

Your minishell handles environment variables by expanding them **while lexing words**:

* `ms_collect_piece()` detects `$`
* `ms_expand_variable()` extracts the variable name
* `ms_env_get_value()` looks it up in `shell->env_list`
* the expanded text is inserted into the token’s `value`
* later parsing may split unquoted whitespace, while quoted expansions stay protected as one argument

So the important design choice is:

> **variable expansion happens before parsing/execution, during word construction.**

I can also do the same explanation as a step-by-step trace for one input like `echo "$HOME $USER"`, showing exactly how `value`, `quoted`, and `argv` change across lexer and parser.
