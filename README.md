*This project has been created as part of the 42 curriculum by jatanaso, marapovi.*

# Minishell

Minishell is a simple UNIX shell written in C.

## Description

The goal of this project is to reproduce a minimal interactive shell that:
- Displays a prompt and reads user input

- Keeps a working command history

- Executes binaries found via `$PATH` or via relative/absolute paths

- Handles quoting and variable expansion (`$VAR`, `$?`)

- Supports redirections (`<`, `>`, `>>`, `<<`) and pipelines (`|`)

- Implements a required set of builtins
	* `echo` with option -n
	* `cd` with only a relative or absolute path
	* `pwd` with no options
	* `export` with no options
	* `unset` with no options
	* `env` with no options or arguments
	* `exit` with no options

- Acts like bash in interactive mode for
	* `ctrl-C`: displays a new prompt on a new line
	* `ctrl-D`: exits the shell
	* `ctrl-\`: does nothing.

## Instructions

To build the project, you need a **C compiler** (cc), **make**, and the **readline development library** for command input and history.

Compile the project using `make`, then run it with `./minishell`.

Build files can be cleaned with `make clean`, `make fclean`, or rebuilt using `make re`.

The shell includes several built-in commands such as
* `echo` (with -n support)
* `cd` (with relative or absolute paths)
* `pwd`
* `export`
* `unset`
* `env`
* `exit`

It also supports input and output redirections (`<`, `>`, `>>`), heredoc (`<<` DELIM), and pipelines (`cmd1 | cmd2 | cmd3`).

Environment variable expansion using `$NAME` and the last command’s exit status using `$?` are implemented as well.

In interactive mode, `Ctrl+C` prints a new prompt on a new line, `Ctrl+D` exits the shell, and `Ctrl+\` is ignored.

## Resources
- Bash Reference Manual (GNU): https://www.gnu.org/software/bash/manual/bash.html
- POSIX Shell Command Language (The Open Group): https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html
- GNU Readline Library documentation: https://tiswww.case.edu/php/chet/readline/rltop.html
- Internation Standard ISO/IEC 9899 - Programming Languges C
- The Linux Programming Interface, by Michael Kerrisk
- Linux man-pages (process & FD syscalls, e.g. execve, pipe, dup2, fork): https://www.man7.org/linux/man-pages/
- AI usage - AI tools were used to support learning and reduce repetitive work. In particular, we used ChatGPT to:
	- clarify expected bash-like behaviors (quoting, expansion, error cases) while cross-checking with official documentation;
	- brainstorm and expand test cases for parsing/redirections/pipes/signals;
	- review edge cases and suggest refactoring ideas (we implemented changes manually and verified behavior ourselves)

## Authors

- marapovi
- jatanaso
