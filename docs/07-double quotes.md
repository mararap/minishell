# **Double quotes**
are handled in the **lexer**, and the implementation follows the expected minishell rule:

* inside `"..."`, characters like space, `|`, `<`, `>` are treated as **literal text**
* but `$` is still allowed to trigger **variable expansion**

## The entry point

In `src/lexer/collect_word.c`, when the lexer is building a word, it checks the current character:

```c
if (wctx->str[*wctx->idx] == '"')
{
	*wctx->was_quoted = 1;
	return (ms_collect_double_quotes(wctx->shell, wctx->str, wctx->idx,
			wctx->allow_expansion));
}
```

So as soon as it sees `"`, it switches from normal word scanning into:

```c
ms_collect_double_quotes(...)
```

That is the function that applies the special double-quote rules.

---

## What `ms_collect_double_quotes()` does

In `src/lexer/collect_word_quotes.c`:

```c
char	*ms_collect_double_quotes(t_shell *shell, char *str, int *idx,
	int allow_expansion)
{
	char	*buf;
	char	*tmp;
	char	*old_buf;

	(*idx)++;
	buf = ft_strdup("");
	if (!buf)
		return (NULL);
	while (str[*idx] && str[*idx] != '"')
	{
		tmp = ms_collect_dq_chunk(shell, str, idx, allow_expansion);
		if (!tmp)
			return (free(buf), NULL);
		ms_mask_ifs(tmp);
		old_buf = buf;
		buf = ft_strjoin(buf, tmp);
		free(old_buf);
		free(tmp);
		if (!buf)
			return (NULL);
	}
	if (str[*idx] == '"')
		(*idx)++;
	return (buf);
}
```

### Step by step

1. skip the opening `"`
2. keep collecting content until the closing `"`
3. join all collected pieces into one final string
4. strip the quotes themselves from the final token value

So the parser never sees `"` as syntax later.
By the time tokenization is done, the quoted content has already been converted into plain token text.

---

## Why metacharacters stop being special inside double quotes

The real rule is inside `ms_collect_dq_chunk()`:

```c
static char	*ms_collect_dq_chunk(t_shell *shell, char *str, int *idx,
	int allow_expansion)
{
	int	start;

	if (str[*idx] == '$' && allow_expansion)
		return (ms_expand_variable(shell, str, idx));
	start = *idx;
	while (str[*idx] && str[*idx] != '"'
		&& !(str[*idx] == '$' && allow_expansion))
		(*idx)++;
	return (ft_substr(str, start, *idx - start));
}
```

This tells us exactly what is special inside double quotes:

* stop at the closing `"`
* stop at `$` if expansion is allowed
* **do not stop** at:

  * space
  * tab
  * `|`
  * `<`
  * `>`
  * `;`
  * `\` (in our project it is not given special shell escaping behavior)

So inside double quotes, those characters are just copied into the current word.

### Example

Input:

```bash
echo "a|b > file"
```

Inside `ms_collect_double_quotes()`:

* `|`
  is not tokenized as `TOKEN_PIPE`
* `>`
  is not tokenized as redirection
* spaces are kept as part of the same argument

So the second argument becomes exactly:

```text
a|b > file
```

not a pipeline or redirection.

---

## Why `$` is the one exception

Inside double quotes, if the current character is `$`, our code does:

```c
if (str[*idx] == '$' && allow_expansion)
	return (ms_expand_variable(shell, str, idx));
```

That means `$` still has meaning there.

The expansion function is in `src/expand/expand_variable.c`:

```c
char	*ms_expand_variable(t_shell *shell, char *str, int *idx)
{
	int		start;
	char	*name;
	char	*value;

	if (str[*idx + 1] == '?')
	{
		*idx = *idx + 2;
		return (ft_itoa(shell->last_exit_status));
	}
	start = *idx + 1;
	while (str[start] && (ft_isalnum(str[start]) || str[start] == '_'))
		start++;
	if (start == *idx + 1)
	{
		*idx = *idx + 1;
		return (ft_strdup("$"));
	}
	name = ft_substr(str, *idx + 1, start - (*idx + 1));
	value = ms_env_get_value(shell->env_list, name);
	free(name);
	*idx = start;
	if (!value)
		return (ft_strdup(""));
	return (ft_strdup(value));
}
```

So inside double quotes:

* `"$HOME"` → expands to the env value
* `"$?"` → expands to last exit status
* `"$NOT_SET"` → expands to `""`
* `"$"` with no valid variable after it → stays literal `$`

That is exactly the intended difference between:

* single quotes: no expansion
* double quotes: `$` expansion allowed

---

## Why spaces inside double quotes do not split the argument

This is a very important part of our implementation.

After every chunk collected inside double quotes, our code does:

```c
ms_mask_ifs(tmp);
```

`ms_mask_ifs()` replaces whitespace with hidden marker bytes:

```c
if (*s == ' ')
	*s = MS_MASK_SPACE;
else if (*s == '\t')
	*s = MS_MASK_TAB;
else if (*s == '\n')
	*s = MS_MASK_NL;
```

So if the content inside quotes contains spaces, tabs, or newlines, they are temporarily masked.

That prevents later field splitting from breaking the quoted string into multiple argv entries.

### Example

Input:

```bash
echo "hello world"
```

Without masking, later processing might split on the space.
With masking, the whole quoted text stays one argument.

Later, when the final argv is built, the parser restores those masked spaces with:

```c
ms_unmask_ifs(tok->value);
```

in `src/parser/handle_words.c`.

So the final argument becomes:

```text
hello world
```

as one single argv entry.

---

## Very important detail: expanded variables inside double quotes are also protected

Because `ms_mask_ifs(tmp)` is applied to **every chunk**, it also protects the result of `$` expansion.

So if:

```bash
VAR="a b"
echo "$VAR"
```

then `ms_expand_variable()` returns `a b`, and after that our code masks the space in that expanded value too.

That means the expansion result stays **one argument** inside double quotes, which is correct shell behavior.

So:

```bash
echo "$VAR"
```

becomes one argv entry:

```text
a b
```

not two separate words `a` and `b`.

---

## How mixed quoted and unquoted pieces are joined

Our lexer builds a full word by collecting pieces and concatenating them.

In `ms_collect_word()`:

```c
while (wctx->str[*wctx->idx] && wctx->str[*wctx->idx] != ' '
	&& wctx->str[*wctx->idx] != '\t' && wctx->str[*wctx->idx] != '|'
	&& wctx->str[*wctx->idx] != '<' && wctx->str[*wctx->idx] != '>')
{
	tmp = ms_collect_piece(wctx);
	...
	ms_join_piece(&buf, tmp);
}
```

So this works:

```bash
echo abc"$HOME"xyz
```

The lexer collects:

* `abc` as plain text
* `$HOME` through double-quote logic
* `xyz` as plain text

and joins them into one word.

That matches shell behavior.

---

## One full walkthrough

Take this input:

```bash
echo "a|$HOME b>c"
```

### Lexing inside the quotes

`ms_collect_double_quotes()` starts after the opening `"`.

It sees:

* `a|` → collected as plain literal text
* `$HOME` → expanded with `ms_expand_variable()`
* ` b>c` → collected as plain literal text

At each step, whitespace is masked.

### Result

The full token value becomes one combined word, something like:

```text
a|/home/user b>c
```

with the internal space protected from splitting.

### What does **not** happen

* `|` does not become a pipe token
* `>` does not become a redirection token
* the internal space does not split the argument
* only `$HOME` is interpreted specially

That is exactly the rule from the subject.

---

## One subtle context flag

Your lexer passes this flag:

```c
wctx.allow_expansion = !lstate->expect_heredoc_delim;
```

So in normal command words, double quotes allow `$` expansion.
But for a heredoc delimiter token, expansion can be intentionally disabled.

That is a separate heredoc rule, not the normal command-word rule, but your code is prepared for that distinction.

---

## Bottom line

Our minishell handles double quotes by routing `"..."` into `ms_collect_double_quotes()`, which:

* consumes everything literally until the next `"`
* does **not** treat `|`, `<`, `>`, or spaces as syntax inside the quotes
* still treats `$` specially through `ms_expand_variable()`
* masks whitespace so quoted content and quoted expansions stay in a single argument

So in our implementation, double quotes correctly mean:

* metacharacters are neutralized inside the quotes
* `$` is still expanded
* the result stays one word unless later concatenated with adjacent unquoted pieces
