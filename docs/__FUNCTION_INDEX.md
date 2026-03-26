# Minishell Complete Function Index

**Purpose:** Reference guide for all 192+ functions in minishell. Organized by module with file locations and descriptions for evaluation prep and codebase navigation.

**How to Use:**
1. Use the **Quick Navigation** below to jump to module of interest
2. Each module table shows: Function Name | Purpose | Why Needed | File Location
3. Reference file paths during peer evaluation to explain your implementation
4. Use as study guide for explaining code architecture to evaluators

---

## Quick Navigation

| Module | Functions | File Pattern |
|--------|-----------|--------------|
| [1. Shell Init & Main](#1-shell-initialization--main) | 5 | `src/shell/main.c`, `src/shell/init.c` |
| [2. Main Loop & Input](#2-main-loop--input) | 5 | `src/shell/loop.c` |
| [3. Lexer & Tokenization](#3-lexer--tokenization) | 8 | `src/lexer/lexer.c`, `src/lexer/token_utils.c` |
| [4. Quote Handling](#4-quote-handling-lexer-sublayer) | 7 | `src/lexer/collect_word_quotes.c` |
| [5. Word Collection](#5-word-collection-lexer-sublayer) | 3 | `src/lexer/collect_word.c` |
| [6. Parser & Commands](#6-parser--command-building) | 14 | `src/parser/*.c` |
| [7. Parser Helpers](#7-parser-helpers) | 5 | `src/parser/parser_helpers.c` |
| [8. Variable Expansion](#8-variable--status-expansion) | 6 | `src/expand/expand_variable.c`, `src/expand/ifs_split.c` |
| [9. Environment](#9-environment-management) | 14 | `src/env/env_list*.c` |
| [10. Builtins General](#10-builtins---general) | 6 | `src/builtin/builtins.c` |
| [11. `echo`](#11-builtins---echo) | 5 | `src/builtin/builtin_echo.c` |
| [12. `cd`](#12-builtins---cd) | 5 | `src/builtin/builtin_cd.c` |
| [13. `pwd`](#13-builtins---pwd) | 1 | `src/builtin/builtin_pwd.c` |
| [14. `export`](#14-builtins---export) | 10 | `src/builtin/builtin_export*.c` |
| [15. `unset`](#15-builtins---unset) | 1 | `src/builtin/builtin_unset.c` |
| [16. `exit`](#16-builtins---exit) | 5 | `src/builtin/builtin_exit*.c` |
| [17. `env`](#17-builtins---env) | 2 | `src/builtin/builtin_env.c` |
| [18. Execution](#18-execution-pipeline--forking) | 18 | `src/exec/*.c` |
| [19. Path Resolution](#19-execution---path-resolution) | 12 | `src/exec/exec_*.c` |
| [20. Redirections](#20-redirections) | 5 | `src/redirections.c` |
| [21. Heredocs](#21-heredocs) | 20 | `src/heredoc/*.c` |
| [22. Signals](#22-signals) | 5 | `src/shell/signals.c` |
| [23. Input & Utilities](#23-input--utilities) | 6 | `src/utils/utils_gnl.c`, `src/lexer/unclosed_quotes.c` |
| [24. Memory & Error](#24-memory--error-utilities) | 13 | `src/utils/utils.c`, `src/utils/utils_command.c` |

---

## 1. Shell Initialization & Main

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `main` | Entry point; initializes shell, detects interactive mode, runs loop, cleanup | Required entry point; initializes all subsystems before main loop | `src/shell/main.c:25` |
| `ms_detect_interactive` | Checks if stdin/stderr are TTY; sets `shell->is_interactive` | Needed to apply different signal behavior and readline logic for interactive vs non-interactive | `src/shell/main.c:17` |
| `ms_init_shell` | Sets up env list from `envp`, bootstraps missing vars (PWD, PATH, SHLVL) | Initializes global shell state and environment from parent process | `src/shell/init.c:35` |
| `ms_free_shell` | Frees env list and clears readline history | Cleanup at exit to avoid leaks and ensure history is cleared | `src/shell/init.c:60` |
| `ms_bootstrap_env` | Ensures PWD, PATH, OLDPWD, SHLVL are initialized | Provides default environment vars if parent didn't supply them | `src/shell/init.c:15` |

---

## 2. Main Loop & Input

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_main_loop` | Main interactive loop; reads lines, parses, executes, updates prompt | Core loop that keeps shell alive until `should_exit` or EOF | `src/shell/loop.c:80` |
| `ms_read_line` | Uses readline (interactive) or `ms_get_next_line` (non-interactive) | Abstracts input source based on interactivity mode | `src/shell/loop.c:63` |
| `ms_handle_line` | Parses line → executes commands → updates status | Orchestrates full line processing: parse + execute + feedback | `src/shell/loop.c:53` |
| `ms_parse_line` | Lexes + parses tokens into command AST (abstract syntax tree); handles empty/syntax error cases | Converts raw input to executable command list | `src/shell/loop.c:26` |
| `ms_execute_commands` | Prepares heredocs, executes pipeline, frees command list | Bridges parse → execution; ensures cleanup | `src/shell/loop.c:15` |

---

## 3. Lexer & Tokenization

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_lex_line` | Main lexer entry; iterates through input, emits token list | Converts raw shell input into structured token stream | `src/lexer/lexer.c:81` |
| `ms_add_redir_token` | Identifies `<`, `>`, `<<`, `>>` and creates redirection tokens | Recognizes and categorizes all redirection operators | `src/lexer/lexer.c:15` |
| `ms_lex_word` | Tokenizes words: calls `ms_collect_word`, stores value + raw form | Splits input into words with both expanded + literal forms | `src/lexer/lexer.c:37` |
| `ms_lex_tokens` | Dispatcher: pipes (`\|`) → `ms_add_redir_token` → words | Orchestrates token type detection and emission | `src/lexer/lexer.c:66` |
| `ms_collect_word` | Builds word by piecing quotes, expansions, literals together | Core word assembly with quote/expansion context awareness | `src/lexer/collect_word.c:79` |
| `ms_free_token_list` | Frees all tokens and associated strings | Cleanup after parse or on error | `src/lexer/lexer.c:103` |
| `ms_tok_new` | Creates new token node with type, value, raw, quoted flag | Factory for token allocation | `src/lexer/token_utils.c:15` |
| `ms_tok_add_back` | Appends token to linked list | Builds token list in order | `src/lexer/token_utils.c:28` |

---

## 4. Quote Handling (Lexer Sublayer)

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_collect_single_quotes` | Extracts content between `'` markers; masks IFS chars (internal field separator - ususally `'\t'`, `' '`, `'\n'`) | Prevents any interpretation inside single quotes | `src/lexer/collect_word_quotes.c:31` |
| `ms_collect_double_quotes` | Extracts content between `"` markers; still allows `$` expansion | Groups text while respecting bash double-quote semantics | `src/lexer/collect_word_quotes.c:63` |
| `ms_collect_locale_quotes` | Handles `$'...'` (locale-specific quoting); delegates to double-quote logic | Supports bash `$'...'` syntax (treated as double-quote variant) | `src/lexer/collect_word_quotes.c:92` |
| `ms_collect_dq_chunk` | Helper for double-quote processing; handles `$` or literal chunks | Processes character sequences within double quotes | `src/lexer/collect_word_quotes.c:49` |
| `ms_mask_ifs` | Replaces space/tab/newline with special markers (0x1F/0x1E/0x1D) | Preserves spaces in quoted text during later IFS splitting | `src/lexer/collect_word_quotes.c:15` |
| `ms_collect_plain_word` | Extracts unquoted plain text until whitespace/operator | Scans literal word segments | `src/lexer/collect_word.c:28` |
| `ms_collect_literal_dollar` | Captures `$` followed by non-identifier chars as literal | Preserves `$` when not followed by variable name | `src/lexer/collect_word.c:40` |

---

## 5. Word Collection (Lexer Sublayer)

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_collect_piece` | Dispatches to quote handler or expansion based on current char | Routes word assembly logic to correct handler (quotes, $, plain) | `src/lexer/collect_word.c:53` |
| `ms_join_piece` | Concatenates word chunks; frees old buffer | Builds full word from fragments | `src/lexer/collect_word.c:15` |
| `ms_expand_variable` | Replaces `$VAR` with env value or `$?` with last status | Core variable/status expansion logic | `src/expand/expand_variable.c:15` |

---

## 6. Parser & Command Building

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_parse_tokens` | Converts token list to command AST (abstract syntax tree, linked list of `t_command`) | Structures tokens into runnable command objects | `src/parser/parser.c:73` |
| `ms_parse_one_command` | Parses one stage of pipeline; handles redirections and args | Builds individual command node with argv + redirections | `src/parser/parser.c:48` |
| `ms_command_new` | Allocates empty command struct | Factory for command nodes | `src/parser/parser.c:15` |
| `ms_command_add_back` | Appends command to pipeline list | Links commands in pipeline order | `src/parser/parser.c:26` |
| `ms_parse_pipe_error` | Reports syntax error when pipe appears as first/last/consecutive token | Validates pipe token placement | `src/parser/parser.c:41` |
| `ms_fill_command` | Iterates through tokens for one command; calls word/redir handlers | Main loop building argv and redirections for one command | `src/parser/fill_command.c:15` |
| `ms_handle_word` | Processes word token; does IFS splitting if needed; builds argv | Converts token to argv entries, splitting on IFS if unquoted | `src/parser/handle_words.c:65` |
| `ms_word_has_ifs` | Checks if word contains unmasked space/tab/newline | Determines if field splitting is needed | `src/parser/handle_words.c:15` |
| `ms_handle_split_word` | Splits word on IFS; adds each field to argv | Handles bash field-splitting semantics | `src/parser/handle_words.c:42` |
| `ms_add_word_to_argv` | Extends argv array and appends word | Grows argv as arguments are parsed | `src/parser/handle_words.c:22` |
| `ms_process_redir_token` | Parses redirection operator + target; validates and creates `t_redir` | Converts redir token to usable redirection object | `src/parser/handle_redirs.c:80` |
| `ms_redir_target_value` | Extracts target filename for redirection; handles IFS splitting and ambiguity | Determines what file a redirection refers to | `src/parser/handle_redirs.c:51` |
| `ms_free_command_list` | Frees all commands, argv arrays, and redirections | Cleanup after execution or on error | `src/parser/free_command.c:28` |
| `ms_free_argv` | Helper to free individual argv array | Part of command list cleanup | `src/parser/free_command.c:15` |

---

## 7. Parser Helpers

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_token_to_str` | Converts token type enum to printable string | Used for syntax error messages | `src/parser/parser_helpers.c:15` |
| `ms_print_syntax_error` | Prints "syntax error near unexpected token `X`" | Required error reporting for invalid syntax | `src/parser/parser_helpers.c:30` |
| `ms_token_to_redir_type` | Converts token type to redir type constant | Maps lexer tokens to redirection categories | `src/parser/parser_helpers.c:75` |
| `ms_create_redir` | Allocates redir struct with type, target, expansion flag | Factory for redirection nodes | `src/parser/parser_helpers.c:44` |
| `ms_redir_add_back` | Appends redir to linked list on command | Builds command's redirection chain | `src/parser/parser_helpers.c:60` |

---

## 8. Variable & Status Expansion

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_expand_variable` | Replaces `$VAR` with env value; `$?` with last status; invalid vars → empty | Core expansion used in double quotes and unquoted context | `src/expand/expand_variable.c:15` |
| `ms_split_ifs_fields` | Splits expanded text on IFS (space/tab/newline); returns argv-like array | Implements field splitting for unquoted expansion results | `src/expand/ifs_split.c:85` |
| `ms_count_ifs_fields` | Counts fields in IFS-delimited string | Used to allocate output array size | `src/expand/ifs_split.c:42` |
| `ms_next_field` | Extracts one IFS-delimited field from string | Builds individual field during splitting | `src/expand/ifs_split.c:62` |
| `ms_is_ifs_delim` | Tests if char is space/tab/newline | IFS delimiter check | `src/expand/ifs_split.c:35` |
| `ms_unmask_ifs` | Converts IFS markers (0x1F/0x1E/0x1D) back to real chars | Restores spaces in quoted text after field splitting | `src/expand/ifs_split.c:19` |

---

## 9. Environment Management

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_env_from_environ` | Parses `envp` into linked list of `t_env_var` nodes | Converts OS environment to internal structure | `src/env/env_list.c:83` |
| `ms_env_get_value` | Looks up variable by name in linked list; returns value or NULL | Used for expansion and lookups | `src/env/env_list_get.c:32` |
| `ms_env_set` | Sets or creates variable; marks as exported or not | Core for `export` and env modification | `src/env/env_list_ops.c:60` |
| `ms_env_unset` | Removes variable from linked list; frees memory | Implements `unset` builtin | `src/env/env_list_unset.c:15` |
| `ms_env_free_list` | Frees all env nodes and names/values | Cleanup at exit | `src/env/env_list_ops.c:44` |
| `ms_env_to_array` | Converts env linked list to `char **envp` for execve | Prepares environment for child process execution | `src/env/env_list_array.c:44` |
| `ms_env_is_valid_name` | Checks if string is valid shell identifier (alphanumeric + underscore, first char alpha/underscore) | Validates env var names for `export` | `src/env/env_list.c:15` |
| `ms_env_find_var` | Helper to locate variable node by name | Used by get/set/unset | `src/env/env_list_ops.c:15` |
| `ms_env_make_node` | Allocates new env var node | Factory for env structs | `src/env/env_list_ops.c:32` |
| `ms_env_append_node` | Adds node to linked list during env construction | Builds env list during init | `src/env/env_list.c:71` |
| `ms_env_split_entry` | Parses "NAME=VALUE" into name and value strings | Used during env init | `src/env/env_list.c:47` |
| `ms_env_exported_count` | Counts exported (non-null) vars for array allocation | Sizing for envp conversion | `src/env/env_list_array.c:15` |
| `ms_env_make_entry` | Builds "NAME=VALUE" string for envp | Formats var for child environment | `src/env/env_list_array.c:29` |
| `ms_env_new_node` | Allocates initial env node during parsing | Factory for env construction | `src/env/env_list.c:31` |

---

## 10. Builtins - General

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_is_builtin` | Returns 1 if command is echo/cd/pwd/env/export/unset/exit | Used to decide if command is builtin or external | `src/builtin/builtin_is_builtin.c:15` |
| `ms_builtin_needs_parent` | Returns 1 if builtin must run in parent (cd/export/unset/exit) | Determines execution context | `src/builtin/builtins.c:15` |
| `ms_run_builtin_child` | Dispatches builtin by name; runs in child or parent | Executes builtin logic | `src/builtin/builtins.c:30` |
| `ms_run_builtin_parent` | Dups/restores stdio around builtin for redirections | Enables redirections on parent builtins | `src/builtin/builtins.c:91` |
| `ms_dup_stdio` | Saves stdin/stdout/stderr file descriptors | Foundation for stdio restoration | `src/builtin/builtins.c:51` |
| `ms_restore_stdio` | Restores saved stdio descriptors | Cleans up after parent builtin execution | `src/builtin/builtins.c:69` |

---

## 11. Builtins - `echo`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_echo` | Implements echo with `-n` flag support | Required builtin; handles newline suppression and SIGPIPE | `src/builtin/builtin_echo.c:75` |
| `ms_is_valid_n_flag` | Checks if arg is `-n`, `-nn`, etc. (only `n` chars after `-`) | Validates `-n` flag format | `src/builtin/builtin_echo.c:15` |
| `ms_echo_skip_flags` | Parses consecutive `-n` flags; tracks newline suppression | Processes all leading `-n` arguments | `src/builtin/builtin_echo.c:31` |
| `ms_echo_print_args` | Outputs arguments separated by spaces | Core output logic | `src/builtin/builtin_echo.c:45` |
| `ms_ignore_sigpipe` | Installs SIG_IGN for SIGPIPE; saves old handler | Prevents crash if output piped to closed command | `src/builtin/builtin_echo.c:62` |

---

## 12. Builtins - `cd`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_cd` | Changes directory; handles home, OLDPWD, PWD updates | Required builtin; must run in parent | `src/builtin/builtin_cd.c:74` |
| `ms_cd_home` | Changes to HOME; updates PWD/OLDPWD | Implements `cd` with no args | `src/builtin/builtin_cd.c:42` |
| `ms_cd_oldpwd` | Changes to OLDPWD and prints it; swaps PWD/OLDPWD | Implements `cd -` | `src/builtin/builtin_cd.c:57` |
| `ms_update_pwd_vars` | Updates PWD to new cwd (current working directory); saves old PWD as OLDPWD | Maintains env consistency | `src/builtin/builtin_cd.c:28` |
| `cd_error_with_path` | Prints cd error message | Error reporting for cd | `src/builtin/builtin_cd.c:15` |

---

## 13. Builtins - `pwd`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_pwd` | Calls getcwd and prints current directory | Required builtin | `src/builtin/builtin_pwd.c:15` |

---

## 14. Builtins - `export`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_export` | Exports vars; no args → prints sorted export format | Required builtin; modifies env | `src/builtin/builtin_export.c:15` |
| `ms_export_one_arg` | Parses and validates one export argument | Processes individual arg | `src/builtin/builtin_export_utils.c:75` |
| `ms_is_valid_identifier` | Checks if string is valid identifier for export | Validates exported var names | `src/builtin/builtin_export_utils.c:15` |
| `ms_parse_export_pair` | Splits "NAME=VALUE" into components | Parses export argument | `src/builtin/builtin_export_utils.c:33` |
| `ms_export_error` | Prints "not a valid identifier" error | Error reporting | `src/builtin/builtin_export_utils.c:57` |
| `ms_export_name_only` | Exports existing var without changing value | Handles `export EXISTING_VAR` | `src/builtin/builtin_export_utils.c:65` |
| `ms_print_export_format` | Prints all exported vars in sorted `export NAME="VALUE"` format | Implements `export` with no args | `src/builtin/builtin_export_print.c:86` |
| `ms_count_env_vars` | Counts vars in env list | Sizing for sort array | `src/builtin/builtin_export_print.c:15` |
| `ms_sort_env_array` | Bubble-sorts env pointers alphabetically by name | Ensures export output is sorted | `src/builtin/builtin_export_print.c:28` |
| `ms_export_sorted_env` | Creates and sorts array of env pointers | Prepares for sorted output | `src/builtin/builtin_export_print.c:52` |

---

## 15. Builtins - `unset`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_unset` | Removes variables from environment | Required builtin; modifies env | `src/builtin/builtin_unset.c:15` |

---

## 16. Builtins - `exit`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_exit` | Parses numeric arg; sets should_exit flag; returns appropriate status | Required builtin; terminates shell | `src/builtin/builtin_exit.c:38` |
| `ms_atoll_strict` | Parses long long with overflow checking; no trailing garbage | Strict numeric parsing for exit | `src/builtin/builtin_exit_parse.c:42` |
| `ms_parse_ull` | Parses unsigned long long; checks bounds | Part of strict parsing | `src/builtin/builtin_exit_parse.c:22` |
| `ms_skip_spaces` | Skips leading whitespace in string | Parsing helper | `src/builtin/builtin_exit_parse.c:15` |
| `ms_exit_too_many` | Prints "too many arguments" error | Error reporting | `src/builtin/builtin_exit.c:15` |

---

## 17. Builtins - `env`

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_builtin_env` | Prints exported env vars in NAME=VALUE format | Required builtin | `src/builtin/builtin_env.c:34` |
| `print_env_entry` | Formats and prints one env entry | Helper for env output | `src/builtin/builtin_env.c:15` |

---

## 18. Execution (PIPELINE & FORKING)

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_execute_pipeline` | Main executor; decides parent-builtin vs forked pipeline; waits for children | Orchestrates command execution | `src/exec/executor.c:59` |
| `ms_count_commands` | Counts stages in pipeline | Used to allocate pid array | `src/exec/executor.c:15` |
| `ms_use_parent_builtin` | Returns 1 if single-command pipeline with cd/export/unset/exit | Optimization: avoid fork for state-changing builtins | `src/exec/executor.c:28` |
| `ms_spawn_pipeline` | Creates pipes and forks all pipeline stages | Builds multi-stage pipeline | `src/exec/executor_spawn.c:87` |
| `ms_create_pipe_if_needed` | Creates pipe before next command if needed | Allocates pipe fd pairs | `src/exec/executor_spawn.c:15` |
| `ms_launch_pipeline_cmd` | Forks and executes one pipeline stage | Spawns individual child process | `src/exec/executor_spawn.c:67` |
| `ms_update_parent_fds` | Closes pipe write end; saves read end for next command | Manages fd inheritance | `src/exec/executor_spawn.c:27` |
| `ms_abort_pipeline` | Kills all spawned children on error; cleanup | Error recovery | `src/exec/executor_spawn.c:42` |
| `ms_prepare_pipeline_signals` | Ignores SIGINT/SIGQUIT in parent during pipeline | Lets children handle signals | `src/exec/executor.c:37` |
| `ms_finish_pipeline` | Closes remaining fds; waits for all children; restores signals | Finalizes pipeline execution | `src/exec/executor.c:45` |
| `ms_fork_and_execute` | Forks; calls `ms_execute_child` in child | Creates child process | `src/exec/exec_run.c:58` |
| `ms_execute_child` | Sets up child signals, fds, redirections, then exec or builtin | Child process main routine | `src/exec/fork.c:83` |
| `ms_dup_child_fds` | Wires stdin from prev_read; stdout to pipe (if next cmd) | Sets up fd redirection in child | `src/exec/fork.c:64` |
| `ms_dup_and_close` | Atomically dups fd to target and closes source | Helper for fd wiring | `src/exec/fork.c:15` |
| `ms_close_heredocs` | Closes heredoc fds for non-current commands in child | Prevents fd pollution in child | `src/exec/fork.c:43` |
| `ms_wait_for_children` | Waits for all children; returns last command's status | Collects child exit statuses | `src/exec/executor_wait.c:45` |
| `ms_status_to_exit` | Converts wait status to shell exit code; handles signals → 128+signum | Exit code extraction | `src/exec/executor_wait.c:15` |
| `ms_report_child_signal` | Prints signal message for interactive execution | User feedback on signal termination | `src/exec/executor_wait.c:24` |

---

## 19. Execution - Path Resolution

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_exec_external_command` | Main external command executor; finds path, prechecks, exec | Executes non-builtin commands | `src/exec/exec_run.c:32` |
| `ms_find_executable` | Resolves command via PATH or direct path | Finds executable file | `src/exec/exec_path.c:47` |
| `ms_join_search_dir` | Joins directory + command name | Builds candidate path | `src/exec/exec_path.c:15` |
| `ms_search_path_dirs` | Iterates PATH dirs; returns first executable found | Searches PATH left-to-right | `src/exec/exec_path.c:22` |
| `ms_exec_run_path` | Calls execve with environment array | Final execution step | `src/exec/exec_run.c:15` |
| `ms_exec_precheck` | Validates executable before execve; catches directory/permission errors | Pre-exec validation | `src/exec/exec_error.c:61` |
| `ms_exec_error_code` | Maps errno to exit code (126/127) | Error code selection | `src/exec/exec_error.c:39` |
| `ms_exec_exit_code` | Returns 126 for dir/exec-format/permission; 127 otherwise | Exit code mapping | `src/exec/exec_error.c:15` |
| `ms_write_exec_message` | Formats error message based on errno | Error message formatting | `src/exec/exec_error.c:22` |
| `ms_exec_directory_status` | Handles case where target is directory | Directory-specific error | `src/exec/exec_error.c:49` |
| `ms_update_underscore` | Sets `_` env var to last command path | Bash compatibility | `src/exec/exec_path.c:67` |
| `ms_adjust_envp_shlvl` | Decrements SHLVL for child process | Maintains SHLVL nesting level | `src/exec/exec_path.c:74` |

---

## 20. Redirections

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_apply_redirections` | Applies all redirections on a command; called in child before exec | Wires input/output before execution | `src/redirections.c:95` |
| `ms_apply_input_redir` | Opens file for reading; dups to stdin | Handles `<` redirection | `src/redirections.c:33` |
| `ms_apply_output_redir` | Opens file for writing/appending; dups to stdout | Handles `>` and `>>` redirections | `src/redirections.c:53` |
| `ms_apply_heredoc_redir` | Dups heredoc fd to stdin | Connects prepared heredoc input | `src/redirections.c:79` |
| `ms_redir_sys_error` | Formats and prints file-level error (open/dup2 failure) | Redirection error reporting | `src/redirections.c:15` |

---

## 21. Heredocs

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_prepare_heredocs` | Main heredoc orchestrator; prepares all heredocs before exec | Builds heredoc files before pipeline fork | `src/heredoc/heredoc_prepare.c:73` |
| `ms_prepare_command_heredocs` | Prepares heredocs for one command in list | Processes heredocs within command | `src/heredoc/heredoc_prepare.c:53` |
| `ms_prepare_one_heredoc` | Prepares single heredoc; calls build; stores fd in redir | Builds one heredoc file | `src/heredoc/heredoc_prepare.c:35` |
| `ms_build_one_heredoc` | Forks child to read heredoc; writes to temp file; returns fd | Core heredoc creation | `src/heredoc/heredoc_build.c:72` |
| `ms_hd_run_child` | Child loop for heredoc: reads lines, writes to temp file | Heredoc child process | `src/heredoc/heredoc_build.c:29` |
| `ms_hd_child_loop` | Reads lines until delimiter; expands and writes if needed | Heredoc reading loop | `src/heredoc/heredoc_read.c:70` |
| `ms_hd_read_line` | Uses readline (interactive) or `ms_get_next_line` (non-interactive) | Reads heredoc input line | `src/heredoc/heredoc_read.c:15` |
| `ms_hd_write_line` | Writes line to heredoc file; expands if `heredoc_expand=1` | Heredoc output with optional expansion | `src/heredoc/heredoc_read.c:49` |
| `ms_hd_warn_eof` | Prints "here-document at line X delimited by end-of-file" | EOF warning | `src/heredoc/heredoc_read.c:33` |
| `ms_hd_wait_child` | Waits for heredoc child; returns status | Collects heredoc child completion | `src/heredoc/heredoc_build.c:40` |
| `ms_hd_finalize_build` | Checks signal, reads return value, reopens for reading, unlinks | Finalizes heredoc and returns fd for stdin wiring | `src/heredoc/heredoc_build.c:49` |
| `ms_hd_child_exit` | Frees resources and exits heredoc child | Heredoc child cleanup | `src/heredoc/heredoc_build.c:15` |
| `ms_close_all_heredocs` | Closes all heredoc fds in command list | Global heredoc cleanup | `src/heredoc/heredoc_prepare.c:15` |
| `ms_expand_heredoc_line` | Expands `$VAR` and `$?` in heredoc line if allowed | Heredoc variable expansion | `src/heredoc/heredoc_expand.c:99` |
| `ms_hd_grow_buf` | Expands buffer if needed | Dynamic buffer for expansion | `src/heredoc/heredoc_expand.c:15` |
| `ms_hd_append_str` | Appends string to expanding buffer | Builds expanded heredoc line | `src/heredoc/heredoc_expand.c:36` |
| `ms_hd_append_status_or_var` | Appends `$?` or `$VAR` expansion | Expansion within heredoc line | `src/heredoc/heredoc_expand.c:51` |
| `ms_hd_append_one` | Appends single character; handles `\$` escaping | Character-by-character expansion | `src/heredoc/heredoc_expand.c:81` |
| `ms_hd_open_tmp` | Creates unique temp file for heredoc; returns fd | Temp file creation | `src/heredoc/heredoc_tmp.c:39` |
| `ms_hd_make_path` | Generates temp file path using counter | Path generation | `src/heredoc/heredoc_tmp.c:26` |
| `ms_restore_signals` | Restores parent or child signal handlers after heredoc | Signal cleanup post-heredoc | `src/heredoc/heredoc_tmp.c:15` |
| `ms_setup_heredoc_child_signals` | Sets up signals for heredoc child (ignores SIGINT initially) | Heredoc child signal config | `src/heredoc/heredoc_tmp.c:71` |
| `ms_discard_pending_heredocs` | On syntax error, reads and discards heredoc bodies | Clears stdin after parse error | `src/shell/loop_syntax.c:36` |
| `ms_discard_one_heredoc_body` | Helper to consume heredoc input until delimiter | Reads one heredoc body | `src/shell/loop_syntax.c:15` |

---

## 22. Signals

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_setup_interactive_signals` | Installs SIGINT handler for prompt; ignores SIGQUIT in interactive mode | Sets up interactive shell signal behavior | `src/shell/signals.c:34` |
| `ms_sigint_interactive` | Handler sets global and newline for readline reset | SIGINT in prompt | `src/shell/signals.c:15` |
| `ms_rl_event_hook` | Readline event hook; resets buffer on global SIGINT signal | Clears prompt buffer safely | `src/shell/signals.c:22` |
| `ms_setup_child_signals` | Resets SIGINT/SIGQUIT to default in child | Child gets normal signal behavior | `src/shell/signals.c:51` |
| `ms_sigint_heredoc` | SIGINT handler for heredoc (returns 130) | Heredoc-specific signal handling | `src/heredoc/heredoc_tmp.c:63` |

---

## 23. Input & Utilities

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_get_next_line` | Custom line reader for non-interactive stdin; handles EINTR | Non-interactive input fallback when readline unavailable | `src/utils/utils_gnl.c:61` |
| `ms_gnl_read_char` | Reads single char; loops on EINTR | Robust single-char read | `src/utils/utils_gnl.c:29` |
| `ms_gnl_append_char` | Appends char to line; reallocates as needed | Builds line character-by-character | `src/utils/utils_gnl.c:48` |
| `ms_chomp_eol` | Removes trailing `\n` and `\r` | Cleans input line | `src/utils/utils_gnl.c:15` |
| `ms_has_unclosed_quotes` | Checks if line has unmatched single or double quotes | Input validation | `src/lexer/unclosed_quotes.c:35` |
| `ms_skip_quote` | Helper to find closing quote | Quote matching | `src/lexer/unclosed_quotes.c:24` |

---

## 24. Memory & Error Utilities

| Function | Purpose | Why Needed | File |
|----------|---------|-----------|------|
| `ms_xmalloc` | malloc wrapper; exits on failure with error message | Safe allocation; prevents NULL dereference bugs | `src/utils/utils.c:15` |
| `ms_strdup_safe` | ft_strdup wrapper; handles NULL safely | Safe string duplication | `src/utils/utils.c:29` |
| `ms_str_arr_len` | Counts strings in NULL-terminated array | Array length calculation | `src/utils/utils.c:36` |
| `ms_free_str_array` | Frees all strings and array itself | Safe array cleanup | `src/utils/utils.c:46` |
| `ms_str_join_three` | Joins three strings; frees intermediate result | Convenience for path/message building | `src/utils/utils.c:61` |
| `ms_print_command_not_found` | Formats and prints "command not found" error | Command-not-found error message | `src/utils/utils_command.c:93` |
| `ms_format_cmd_name` | Escapes non-printable chars for display; adds `$'...'` if needed | Safe command name formatting | `src/utils/utils_command.c:70` |
| `ms_needs_ansi_quote` | Checks if string needs `$'...'` quoting | Determines ANSI quoting need | `src/utils/utils_command.c:15` |
| `ms_diag_named_escape` | Maps control chars to escape names (`\n`, `\t`, etc.) | Escape sequence generation | `src/utils/utils_command.c:26` |
| `ms_diag_put_escape` | Outputs one character as escape or octal | Character escaping | `src/utils/utils_command.c:48` |
| `ms_cd_output` | Formats and prints cd-specific error | Error output for cd command | `src/builtin/builtin_cd_output.c:15` |
| `ms_exit_numeric_error` | Prints "numeric argument required" error | Error reporting | `src/builtin/builtin_exit.c:23` |
| `ms_mark_exit` | Sets `should_exit` flag and returns status | Signals main loop to exit | `src/builtin/builtin_exit.c:32` |

---

## Summary

**Total Functions: 192+** across **24 functional domains**

**Key Integration Points:**
- **main** → **ms_main_loop** → **ms_read_line** → **ms_parse_line** → **ms_lex_line** → **ms_parse_tokens** → **ms_prepare_heredocs** → **ms_execute_pipeline** → **ms_wait_for_children**

Each function integrates into the larger shell pipeline with clear ownership of inputs, outputs, and cleanup responsibilities.

**For Peer Evaluation:**
- Reference specific file + line for each explanation
- Use table rows as quick lookup during defense
- Print or bookmark for exam-day reference
