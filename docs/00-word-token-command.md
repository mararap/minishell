There are three different levels of the same input.

## The short version

* **word** = a piece of shell text like `echo`, `hello`, `$HOME`, `"a b"`, `file.txt`
* **token** = the lexer’s packaged unit, with a type like `TOKEN_WORD`, `TOKEN_PIPE`, `TOKEN_REDIR_OUT`
* **command** = the parser’s executable unit: one pipeline segment with `argv` and redirections

So the flow is:

```text
input line
-> words/operators are recognized
-> tokens are created
-> tokens are grouped into commands
-> commands are executed
```

---

## 1) What a **token** is in your project

Our token struct is:

```c
typedef struct s_token
{
	char			*value;
	char			*raw;
	t_token_type	type;
	int				quoted;
	struct s_token	*next;
}	t_token;
```

And token types are:

```c
TOKEN_WORD
TOKEN_PIPE
TOKEN_REDIR_IN
TOKEN_REDIR_OUT
TOKEN_REDIR_APPEND
TOKEN_HEREDOC
```

So a **token** is the lexer’s formal object for one syntactic item.

Examples:

* `echo` -> token of type `TOKEN_WORD`
* `|` -> token of type `TOKEN_PIPE`
* `>` -> token of type `TOKEN_REDIR_OUT`
* `<<` -> token of type `TOKEN_HEREDOC`

### Important

A token is not always a word.

Some tokens are operators:

* pipe
* redirections

Only some tokens are `TOKEN_WORD`.

---

## 2) What a **word** is in our project

In our lexer, a **word** is the text collected by `ms_collect_word()` before it gets wrapped into a token.

In `lexer.c`:

```c
word = ms_collect_word(&wctx);
...
ms_tok_add_back(tokens, ms_tok_new(TOKEN_WORD, word, raw, quoted));
```

So a word is basically:

> “text that belongs together as one shell word”

Examples of things that become a word:

* `ls`
* `abc`
* `$HOME`
* `'a b'`
* `"hello $USER"`
* `outfile.txt`

### But:

A word exists in two forms inside the token:

* `raw` = original text from the line
* `value` = processed text after quote handling / expansion logic

For example, input:

```bash
echo "$HOME"
```

might give a token like:

* `raw = "\"$HOME\""`
* `value = "/home/user"`

So in our minishell, “word” often means the **content tokenized as `TOKEN_WORD`**, not just raw characters.

---

## 3) What a **command** is in our project

Our command struct is:

```c
typedef struct s_command
{
	char			**argv;
	t_redir			*redirections;
	struct s_command	*next;
}	t_command;
```

So a **command** is no longer a lexical object.
It is a parsed executable unit.

A command contains:

* `argv` -> arguments to run
* `redirections` -> `<`, `>`, `>>`, `<<`
* `next` -> next command in a pipeline

### Example

Input:

```bash
echo hello > out
```

becomes one command roughly like:

```text
argv = ["echo", "hello", NULL]
redirections = [ > "out" ]
next = NULL
```

For:

```bash
echo hello | wc -c
```

you get two commands:

#### command 1

```text
argv = ["echo", "hello", NULL]
redirections = none
```

#### command 2

```text
argv = ["wc", "-c", NULL]
redirections = none
```

linked through `next`.

So a **command** is closer to:

> “one simple command between pipes”

not the entire input line.

---

# The key difference

## Word

A **word** is just shell text that belongs together.

Examples:

* `echo`
* `hello`
* `"hello world"`
* `$USER`

It is still at the lexical/content level.

---

## Token

A **token** is the lexer’s typed node.

Examples:

* `TOKEN_WORD("echo")`
* `TOKEN_WORD("hello")`
* `TOKEN_PIPE`
* `TOKEN_REDIR_OUT`
* `TOKEN_WORD("out")`

So token = word or operator, with metadata.

---

## Command

A **command** is the parser’s grouped executable structure.

Examples:

* command with `argv = ["echo", "hello"]`
* command with redirection `> out`
* command linked to another command by a pipe

So command = something the executor can run.

---

# A concrete example

Take this input:

```bash
echo "hi there" > out | cat
```

## A) Words seen in the line

Conceptually, the word-like parts are:

* `echo`
* `"hi there"`
* `out`
* `cat`

Operators are:

* `>`
* `|`

---

## B) Tokens produced by the lexer

Our lexer turns that into something like:

```text
TOKEN_WORD("echo")
TOKEN_WORD("hi there")
TOKEN_REDIR_OUT
TOKEN_WORD("out")
TOKEN_PIPE
TOKEN_WORD("cat")
```

Notice:

* `"hi there"` becomes one `TOKEN_WORD`
* `>` is its own token
* `|` is its own token

---

## C) Commands produced by the parser

### first command

```text
argv = ["echo", "hi there", NULL]
redirections = [ > "out" ]
```

### second command

```text
argv = ["cat", NULL]
redirections = none
```

So the parser uses tokens to build commands.

---

# Where each stage happens in our code

## Word collection

This is lexer-side:

* `ms_collect_word()`
* `ms_collect_single_quotes()`
* `ms_collect_double_quotes()`

This stage decides:

* what text belongs to one word
* how quotes affect it
* whether `$` expands

---

## Token creation

Still lexer-side:

* `ms_tok_new(...)`
* `ms_lex_line(...)`

This stage decides:

* is this a `TOKEN_WORD`?
* or `TOKEN_PIPE`?
* or `TOKEN_REDIR_OUT`?
* etc.

---

## Command building

Parser-side:

* `ms_parse_tokens(...)`
* `ms_fill_command(...)`
* `ms_handle_word(...)`
* `ms_process_redir_token(...)`

This stage decides:

* which words go into `argv`
* which words are redirection targets
* where one command ends at `|`

---

# One subtle point: not every word becomes an argv entry directly

This is important in our parser.

In `ms_handle_word()`, a `TOKEN_WORD` may:

* become one argv entry
* split into multiple argv entries if field splitting applies
* become an empty string if quoted empty text existed
* be consumed as a redirection target in redirection parsing instead of becoming argv

So:

* **word** is lexical text
* **token** is the typed lexical node
* **command** is the parsed execution structure

but a word does not always map 1:1 to final `argv`.

---

# The easiest mental model

Think of it like this:

## word = “text chunk”

`hello`

## token = “labeled chunk”

`TOKEN_WORD("hello")`

## command = “runnable unit”

`argv = ["echo", "hello"]`

---

# In one sentence

In our minishell, a **word** is a piece of shell text collected by the lexer, a **token** is the lexer’s typed object for either a word or an operator, and a **command** is the parser’s higher-level structure that groups word tokens and redirections into one executable pipeline stage.
