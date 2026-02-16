
check for leaks in lexer
free everything properly
shorten functions >25 lines
split files with >5 functions
insert headers
rewrite code


20260206 Notes:
- to research why we use one global variable;
- to check how to implement tab
- Juliyan exit test:
	- to add the argument and :  in the "numeric argument required" error
	- to add jumanshi a front of the :too many argument"
	- exit $UNSET should exit properly
	
- Juliyan to check when testing ctrl-\ is there a message reqired to be printed "Quit (core dumped)" If the running command does not catch or ignore SIGQUIT:
The process terminates
The shell prints a message

Double Quotes:

	jumanshe$ echo "echo lol.c  cat> lol.c"
	echo lol.c cat
	jumanshe$ echo "echo lol.c |  cat lol.c"
	cat: lol.c: No such file or directory
	jumanshe$ echo "echo lol.c | cat lol.c"
	cat: lol.c: No such file or directory
	
Single Quotes: should not print the actual user. Currently it is not escape symbol.
	jumanshe$ echo '$USER'
	jatanaso
	jumanshe$ 
	
	
The message after unset PATH should be "jumanshe: ls: No such file or directory"

Redirections are not implemented.



20260208 Notes:
- 
20260216 - Maria continuing fixing issues

What to tackle next?

Looking at your test results (750/986 passing, 76%), and your test files, here's my recommendation:
Start with: 1_builtins_env.sh ✅

Why?

    Simplest builtin - just print the environment
    No complex logic - no parsing arguments, no side effects
    Foundation for everything else - you need t_env list working
    Quick win - builds confidence

Then: 1_builtins_export.sh (no args) ✅

    export with no args = print all exported variables
    Still simple, reuses env printing logic
    Don't worry about export VAR=value yet - that's harder

Then: 1_builtins_pwd.sh ✅

    Super simple: getcwd() + printf
    No arguments to parse (except error handling for -L etc.)

Then: 1_builtins_echo.sh ⚠️

    Trickier than it looks!
    -n flag parsing
    Multiple -n flags: echo -n -n -n hello (no newline)
    Invalid flags: echo -nnn -na hello (first is -n, second prints -na)

Then: 1_scmds.sh (simple commands) 🎯

    This is where execution really starts!
    No pipes, just single commands
    Practice: fork, execve, waitpid, exit status
    Handle: PATH searching, command not found (127)





