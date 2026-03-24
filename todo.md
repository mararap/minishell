
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

## Execution Layer - Maria's Tasks

### Phase 1: Simple Builtins
- [x] `env` - print environment
- [x] `pwd` - print working directory
- [x] `export` (no args) - print exported vars

### Phase 2: Medium Builtins
- [x] `echo` - with `-n` flag handling
- [x] `unset` - remove environment variables
- [ ] `export VAR=value` - set variables
- [x] `cd` - change directory, update PWD/OLDPWD
- [x] `exit` - numeric parsing, overflow handling

### Phase 3: Execution Engine
- [x] `1_scmds.sh` - simple commands (fork/exec)
- [x] `2_path_check.sh` - PATH searching
- [x] `1_redirs.sh` - file redirections (>, >>, <, <<)
- [ ] `1_pipelines.sh` - multiple commands with pipes
/*from www.geeksforgeeks.com: Pipes behave FIFO(First in First out), Pipe behave like a queue data structure. Size of read and write don’t have to match here. check implementation! see test case line 180 in 1_pipelines.sh*/

### Phase 4: Final Integration
- [ ] `9_go_wild.sh`
- [ ] `2_correction.sh`

- [ ] remove -g from Makefile (?)
- [ ] norminette & split functions / files


#Vesta's test

'export a' should not put a to env list, but only to export
'export a=' should be added to env list
'export la="a b c"' 
export $la="haha"
env' should set la=a b c and c=haha, but not set a= or b=


