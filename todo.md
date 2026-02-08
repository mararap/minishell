
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





