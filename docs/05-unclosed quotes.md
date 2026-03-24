Yes. In your `minishell.zip`, this requirement is handled in **two different ways**:

1. **unclosed quotes are detected early and rejected**
2. **unsupported shell metacharacters like `\` and `;` are never given special syntax meaning in the lexer/parser**

## 1) Unclosed quotes are rejected before lexing/parsing

The main guard is in `src/shell/loop.c`, inside `ms_parse_line()`:

```c
if (ms_has_unclosed_quotes(line))
	return (ms_syntax_error(shell));
```

So before the line is tokenized at all, your shell scans it for broken quotes.

### Where the quote check happens

In `src/lexer/unclosed_quotes.c`:

```c
int	ms_has_unclosed_quotes(char *line)
{
	int	i;

	i = 0;
	while (line && line[i])
	{
		if (line[i] == '\'')
		{
			if (ms_skip_quote(line, &i, '\''))
				return (1);
		}
		else if (line[i] == '"' || (line[i] == '$' && line[i + 1] == '"'))
		{
			if (line[i] == '$')
				i++;
			if (ms_skip_quote(line, &i, '"'))
				return (1);
		}
		else
			i++;
	}
	return (0);
}
```

And `ms_skip_quote()` walks forward until it finds the matching closing quote.
If it reaches end-of-line first, it returns failure.

So:

* `echo "hello` → detected as **unclosed**
* `echo 'abc` → detected as **unclosed**
* `echo $"HOME` → also detected as **unclosed**

### What happens after detection

`ms_syntax_error(shell)` does:

* print syntax error
* set exit status to `2`
* if non-interactive, mark shell to exit

From `src/lexer/unclosed_quotes.c`:

```c
t_command	*ms_syntax_error(t_shell *shell)
{
	ms_print_syntax_error(NULL);
	shell->last_exit_status = 2;
	if (!shell->is_interactive)
		shell->should_exit = 1;
	return (NULL);
}
```

So unclosed quotes are **not interpreted as partial strings** and are **not executed**.
They stop the line before lexer/parser/executor proceeds.

---

## 2) `\` and `;` are not treated as shell operators

This is achieved mostly by **what your lexer does not recognize**.

### Your token types are only these

In `include/minishell.h`:

```c
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}	t_token_type;
```

Notice what is **missing**:

* no `TOKEN_SEMICOLON`
* no `TOKEN_BACKSLASH`
* no `TOKEN_AND`
* no `TOKEN_OR`
* no parentheses token
* no wildcard token

So the parser only knows about:

* words
* `|`
* `<`
* `>`
* `>>`
* `<<`

That is already the first reason `;` and `\` are not interpreted specially.

---

## 3) Why `;` becomes an ordinary character

The lexer only splits words on these characters:

* space / tab
* `|`
* `<`
* `>`
* quotes
* `$` for expansion handling

From `src/lexer/collect_word.c`:

```c
while (str[*idx] && str[*idx] != ' ' && str[*idx] != '\t'
	&& str[*idx] != '|' && str[*idx] != '<' && str[*idx] != '>'
	&& str[*idx] != '\'' && str[*idx] != '"' && str[*idx] != '$')
	(*idx)++;
```

There is **no check for `;`** here.

So semicolon stays inside a normal word.

### Example

Input:

```bash
echo hello;world
```

Tokenization behavior:

* `echo` → word
* `hello;world` → word

It is **not** split into two commands.

Another example:

```bash
;
```

Since `;` is not a special token, lexer treats it as a word token with value `";"`.

So your shell does **not** execute it as a command separator like Bash would.
It treats it like ordinary text.

---

## 4) Why `\` becomes an ordinary character

Same reason: backslash is never given escape semantics.

Again from `ms_collect_plain_word()`, the stop list does **not** include `\`, so backslash is just copied into the token as-is.

There is also **no dedicated backslash-processing function** in the lexer.
So your shell does not do Bash-style escaping such as:

* `\ ` for escaped space
* `\|` for escaped pipe
* `\"` outside quotes
* line continuation with trailing backslash

None of that is implemented.

### Example

Input:

```bash
echo a\ b
```

In Bash, backslash escapes the space, so that would usually become one argument: `a b`.

In your minishell, backslash is literal, so the lexer sees:

* `echo`
* `a\`
* `b`

because the space still splits the word.

So this is exactly “do not interpret backslash as a special shell character.”

---

## 5) Quotes are interpreted only when properly closed

Once the line passes the unclosed-quote precheck, the lexer can safely use the quote collectors:

* `ms_collect_single_quotes()`
* `ms_collect_double_quotes()`
* `ms_collect_locale_quotes()`

These are in `src/lexer/collect_word_quotes.c`.

For example:

```c
char	*ms_collect_single_quotes(char *str, int *idx)
{
	(*idx)++;
	start = *idx;
	while (str[*idx] && str[*idx] != '\'')
		(*idx)++;
	...
	if (str[*idx] == '\'')
		(*idx)++;
	return (tmp);
}
```

So closed quotes are used for grouping/content extraction, but unclosed quotes never get this far because `ms_has_unclosed_quotes()` blocks them first.

That separation is good design:

* **phase 1:** reject broken quotes
* **phase 2:** collect valid quoted content

---

## 6) Parser also does not know `;` or `\`

Even after lexing, the parser only handles:

* words
* redirections
* pipes

From `src/parser/fill_command.c`:

```c
while (*cursor && (*cursor)->type != TOKEN_PIPE)
{
	tok = *cursor;
	if (tok->type == TOKEN_WORD)
		ms_handle_word(cmd, cursor, tok);
	else if (ms_process_redir_token(cmd, cursor, tok) < 0)
		return (-1);
}
```

So if a token contains `;` or `\`, it is just a `TOKEN_WORD`.

No parser branch gives those characters shell-control meaning.

---

## 7) Resulting behavior in practice

### Unclosed quote

Input:

```bash
echo "hello
```

Flow:

* `ms_has_unclosed_quotes()` returns `1`
* `ms_syntax_error()` runs
* no lexing/parsing/execution

### Semicolon

Input:

```bash
echo hi; ls
```

Your shell does **not** treat `;` as a command separator.

Likely tokenization becomes:

* `echo`
* `hi;`
* `ls`

So `;` stays literal text.

### Backslash

Input:

```bash
echo a\ b
```

Your shell does **not** use `\` as an escape.
So arguments become roughly:

* `a\`
* `b`

instead of one combined `a b`.

---

## 8) In one sentence

Your minishell handles this requirement by **explicitly rejecting unclosed quotes before lexing**, and by **never defining unsupported characters like `\` and `;` as shell syntax at all**, so they remain ordinary word characters rather than operators.

There is one practical caveat: this means the shell is intentionally **less Bash-like** for those unsupported features, which is exactly what the project requirement wants.
