**Single quotes** are handled by the **lexer**.

## The main idea

When the lexer sees a `'`, it switches into a **literal collection mode**:

* it reads everything until the next `'`
* it does **not** treat `|`, `<`, `>`, `$`, spaces, tabs, or newlines as special while inside that quoted part
* it returns that content as plain text, without the surrounding quotes

So `'abc|$HOME > file'` becomes one literal piece of a word, not syntax.

---

## Where this starts

In `src/lexer/collect_word.c`, `ms_collect_piece()` decides what kind of piece it is reading:

```c
if (wctx->str[*wctx->idx] == '\'')
{
	*wctx->was_quoted = 1;
	return (ms_collect_single_quotes(wctx->str, wctx->idx));
}
```

So as soon as the current character is `'`, our shell does **not** continue normal word scanning.
It explicitly delegates to:

```c
ms_collect_single_quotes(...)
```

That is the entry point for single-quoted text.

---

## What `ms_collect_single_quotes()` does

In `src/lexer/collect_word_quotes.c`:

```c
char	*ms_collect_single_quotes(char *str, int *idx)
{
	int		start;
	char	*tmp;

	(*idx)++;
	start = *idx;
	while (str[*idx] && str[*idx] != '\'')
		(*idx)++;
	tmp = ft_substr(str, start, *idx - start);
	if (!tmp)
		return (NULL);
	ms_mask_ifs(tmp);
	if (str[*idx] == '\'')
		(*idx)++;
	return (tmp);
}
```

### Step by step

1. `(*idx)++;`
   skip the opening `'`

2. `while (str[*idx] && str[*idx] != '\'') (*idx)++;`
   keep moving until the closing `'`

3. `ft_substr(...)`
   extract the inside text only

4. `ms_mask_ifs(tmp);`
   protect spaces/tabs/newlines inside the quoted text

5. skip the closing `'`

So `'hello world'` becomes the string:

```text
hello world
```

without the quote characters themselves.

---

## Why metacharacters stop being special inside single quotes

Because inside `ms_collect_single_quotes()`, the loop only stops at the **next single quote**:

```c
while (str[*idx] && str[*idx] != '\'')
	(*idx)++;
```

Notice what it does **not** stop on:

* `|`
* `<`
* `>`
* `$`
* space
* tab
* newline

That means these characters are consumed as ordinary content.

### Example 1

Input:

```bash
echo 'a|b'
```

What happens:

* lexer sees `'`
* `ms_collect_single_quotes()` reads `a|b`
* `|` is not tokenized as `TOKEN_PIPE`

So this becomes one normal argument: `a|b`

not:

```bash
echo a | b
```

---

### Example 2

Input:

```bash
echo '$HOME'
```

Inside single quotes, `$HOME` is collected literally.

Why? Because `ms_collect_single_quotes()` never calls `ms_expand_variable()`.

Compare that with double quotes, where our code does call expansion logic.

So:

```bash
'$HOME'
```

stays:

```text
$HOME
```

and is **not expanded**.

---

### Example 3

Input:

```bash
echo '<file >out | grep'
```

All of this is literal text inside the word.
The lexer will not create redirection or pipe tokens from those characters.

---

## Why spaces inside single quotes stay in the same argument

This is an important extra detail.

After extracting the quoted text, our code does:

```c
ms_mask_ifs(tmp);
```

`ms_mask_ifs()` replaces internal whitespace with hidden marker bytes:

```c
if (*s == ' ')
	*s = MS_MASK_SPACE;
else if (*s == '\t')
	*s = MS_MASK_TAB;
else if (*s == '\n')
	*s = MS_MASK_NL;
```

This matters because later, during argument handling, our shell may split words on IFS whitespace.
But quoted whitespace must **not** split the argument.

So for:

```bash
echo 'hello world'
```

the space inside the quotes is masked first, so later it does **not** break into two argv entries.

Only at the end, right before storing the final argument, it is restored with:

```c
ms_unmask_ifs(tok->value);
```

in `src/parser/handle_words.c`.

That is how the shell preserves:

```bash
'hello world'
```

as **one argument**, not two.

---

## How quoted text becomes part of a normal word

Single-quoted content is not necessarily a whole token by itself.
It can be joined with surrounding unquoted text.

In `ms_collect_word()`:

```c
while (...)
{
	tmp = ms_collect_piece(wctx);
	...
	ms_join_piece(&buf, tmp);
}
```

So pieces are concatenated.

### Example

Input:

```bash
echo abc'def'ghi
```

Pieces:

* `abc` from plain word collection
* `def` from single quotes
* `ghi` from plain word collection

Then they are joined into one final word:

```text
abcdefghi
```

That matches normal shell behavior.

---

## Why the parser never sees quote syntax anymore

By the time parsing starts, the single quotes are already removed and their content has already been merged into a `TOKEN_WORD`.

In `src/lexer/lexer.c`:

```c
ms_tok_add_back(tokens, ms_tok_new(TOKEN_WORD, word, raw, quoted));
```

So the parser receives:

* `value` = processed word content
* `raw` = original text from input
* `quoted` = whether quotes were involved

That means the parser is no longer interpreting `'` as syntax.
The quoting effect has already been applied during lexing.

---

## Why empty single quotes still work

Our token stores whether it was quoted:

```c
tok->quoted = quoted;
```

Later in `src/parser/handle_words.c`:

```c
if ((tok->value[0]) == '\0')
{
	if (tok->quoted)
		cmd->argv = ms_add_word_to_argv(cmd->argv, ft_strdup(""));
	...
}
```

So this works correctly:

```bash
echo ''
```

Because:

* the quoted content is empty
* but `quoted == 1`
* so the shell keeps it as an empty string argument

instead of dropping it completely.

That is an important and correct detail.

---

## Unclosed single quotes are rejected earlier

Before lexing, our shell checks for unclosed quotes with `ms_has_unclosed_quotes()`.

So something like:

```bash
echo 'abc
```

never reaches `ms_collect_single_quotes()` as a valid tokenization case.
It is rejected as a syntax error first.

---

## One concrete walkthrough

Take this input:

```bash
echo 'a|$HOME b'
```

### Lexer behavior

* `echo` → normal word
* `'a|$HOME b'` → `ms_collect_single_quotes()`

Inside the quotes:

* `|` is literal
* `$HOME` is literal
* space is masked so it stays inside the same argument

Final token value becomes effectively:

```text
a|$HOME b
```

as one quoted word.

### Parser behavior

Later, when building `argv`:

* it does not split that internal space
* it restores the masked space
* resulting argv entry is exactly:

```text
a|$HOME b
```

So execution sees:

```c
argv[0] = "echo"
argv[1] = "a|$HOME b"
```

---

## Bottom line

Our minishell handles single quotes by sending `'...'` into `ms_collect_single_quotes()`, which reads everything literally until the matching closing quote, strips the quote characters, masks internal whitespace so it is not field-split later, and never performs expansion or metacharacter tokenization on the quoted content.

So in our implementation, single quotes correctly make the enclosed sequence **literal**:

* no pipe parsing
* no redirection parsing
* no variable expansion
* no whitespace splitting inside the quoted part
